# ps5-payload-loader
Homebrew app to load payloads elf/bin from the PS5 internal drive or external usb drive. This is intended to be used with the [PS5-Jar-Loader](https://github.com/cy33hc/ps5-jar-loader/releases) where loading some payloads like etaHEN and kstuff from the Blue Ray drive app will crash the PS5 when the Disc Player is closed. This avoids having to send payload from laptop or phone.

| ![image](https://github.com/user-attachments/assets/ffbde9b6-aa43-465a-b809-a4c3faf3849d) | ![image](https://github.com/user-attachments/assets/30838680-5f38-48c0-a10c-b1f14e8b7b8e) |
|-----------|-------------|

## First time setup
 - Download [PS5-Jar-Loader ISO](https://github.com/cy33hc/ps5-jar-loader/releases) and burn it to a BD-R/BD-RE disk.
 - Put disk into PS5 and then start "PS5 JAR Loader" from the "Disc Player"
 - Run the utmx expliot, elf-loader followed by the websrv and ftpsrv payloads
 - Download the [Homebrew Launcher Pkg](https://github.com/ps5-payload-dev/websrv/releases/download/v0.22/IV9999-FAKE00000_00-HOMEBREWLOADER01.pkg) and install it
 - Download [Payload Loader Homebrew App](https://github.com/cy33hc/ps5-payload-loader/releases/download/1.00/payload-loader.zip). Extract the contents of the ZIP and upload it to the PS5 `/data/homebrew` folder.
 - Start the "Homebrew Launcher" app and then select the "Payload Loader" menu item to load the payloads. By default I have included the etaHEN-2.0b, kstuff-1.4 and ftpsrv-0.11.3 payloads

## Normal usage
 - Put disk into PS5 and then start "PS5 JAR Loader" from the "Disc Player"
 - Run the utmx expliot, elf-loader followied by the websrv payloads
 - Start the "Homebrew Launcher" app and then go to "Payload Loader" menu item to load the payloads you want.

## How to update/add payloads
 - Download the payload you want and ftp it to the PS5 `/data/homebrew/payload-loader/payloads` folder
 - The payload will be automatically included in the "Payload Loader" menu item
