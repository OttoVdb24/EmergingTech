import asyncio
import struct
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Arduino-Sensor"
CHAR_UUID   = "12345678-1234-1234-1234-123456789013"


def on_notification(sender, data: bytearray):
    waarde = struct.unpack("<f", data)[0]
    print(f"Ontvangen waarde: {waarde:.4f}")


async def main():
    print("Scannen naar BLE-apparaten...")

    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)
    if device is None:
        print(f'Apparaat "{DEVICE_NAME}" niet gevonden.')
        return

    print(f"Gevonden: {device.name} ({device.address})")

    async with BleakClient(device) as client:
        print("Verbonden! Druk op Ctrl+C om te stoppen.\n")

        await client.start_notify(CHAR_UUID, on_notification)

        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            pass
        finally:
            await client.stop_notify(CHAR_UUID)
            print("\nVerbinding verbroken.")


if __name__ == "__main__":
    asyncio.run(main())