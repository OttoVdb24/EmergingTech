import asyncio
import struct
import threading
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Arduino-Pommeldoro"
CHAR_UUID   = "12345678-1234-1234-1234-123456789013"


class BLE:
    def __init__(self, device_name=DEVICE_NAME, char_uuid=CHAR_UUID):
        self.device_name = device_name
        self.char_uuid = char_uuid

        self.waarde = None          # Laatste ontvangen waarde
        self.verbonden = False
        self.fout = None            # Eventuele foutmelding

        self._client = None
        self._loop = None
        self._thread = None
        self._stop_event = threading.Event()

    # ------------------------------------------------------------------ #
    #  Publieke functies — roep deze aan vanuit pygame                     #
    # ------------------------------------------------------------------ #

    def verbind(self):
        """Start de BLE-verbinding in een achtergrond-thread."""
        self._stop_event.clear()
        self._thread = threading.Thread(target=self._run_loop, daemon=True)
        self._thread.start()

    def verbreek(self):
        """Verbreek de verbinding netjes."""
        self._stop_event.set()
        if self._loop and self._loop.is_running():
            asyncio.run_coroutine_threadsafe(self._stop(), self._loop)
        if self._thread:
            self._thread.join(timeout=5)

    def lees_waarde(self):
        """Geeft de laatste ontvangen float-waarde terug (of None)."""
        return self.waarde

    def is_verbonden(self):
        """Geeft True terug als de BLE-verbinding actief is."""
        return self.verbonden

    # ------------------------------------------------------------------ #
    #  Interne async-logica                                                #
    # ------------------------------------------------------------------ #

    def _run_loop(self):
        self._loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self._loop)
        try:
            self._loop.run_until_complete(self._main())
        finally:
            self._loop.close()

    def _on_notification(self, sender, data: bytearray):
        self.waarde = struct.unpack("<f", data)[0]

    async def _main(self):
        print("Scannen naar BLE-apparaten...")
        try:
            device = await BleakScanner.find_device_by_name(
                self.device_name, timeout=10.0
            )
        except Exception as e:
            self.fout = str(e)
            return

        if device is None:
            self.fout = f'Apparaat "{self.device_name}" niet gevonden.'
            print(self.fout)
            return

        print(f"Gevonden: {device.name} ({device.address})")

        async with BleakClient(device) as client:
            self._client = client
            self.verbonden = True
            print("Verbonden!\n")

            await client.start_notify(self.char_uuid, self._on_notification)

            while not self._stop_event.is_set():
                await asyncio.sleep(0.1)

            await client.stop_notify(self.char_uuid)
            self.verbonden = False
            print("Verbinding verbroken.")

    async def _stop(self):
        self._stop_event.set()