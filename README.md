# ps5-payloader-loader
Homebrew app to load payloads elf/bin from the PS5 internal drive or external usb drive. This is intended to be used with the [PS5-Jar-Loader](https://github.com/cy33hc/ps5-jar-loader/releases) where loading some payloads like etaHEN and kstuff from the Blue Ray drive app will crash the PS5.

| ![screenshot1](https://github.com/user-attachments/assets/d0ca0ff8-44ad-4129-a153-9cadc0a94bbe)  |  ![screenshot2](https://github.com/user-attachments/assets/346ed2f2-42bc-42a0-bda1-670893bf75a5) |
|-----------|-------------|

## First time setup
 - Download [PS5-Jar-Loader ISO](https://github.com/cy33hc/ps5-jar-loader/releases) and burn it to a BD-R/BD-RE disk.
 - Put disk into PS5 and then start "PS5 JAR Loader" from the "Disc Player"
 - Run the utmx expliot, elf-loader followied by the websrv and ftpsrv payloads
 - Download the [Homebrew Launcher Pkg](https://github.com/ps5-payload-dev/websrv/releases/download/v0.22/IV9999-FAKE00000_00-HOMEBREWLOADER01.pkg) and install it
 - Download [Payload Loader Homebrew App](https://github.com/cy33hc/ps5-payloader-loader/releases/download/1.00/payload-loader.zip). Extract the contents of the ZIP and upload it to the PS5 `/data/homebrew` folder.
 - Start the "Homebrew Launcher" app and then select the "Payload Loader" menu item to load the payloads. By default I have included the etaHEN-2.0b, kstuff-1.4 and ftpsrv-0.11.3 payloads

## Nornal usage
 - Put disk into PS5 and then start "PS5 JAR Loader" from the "Disc Player"
 - Run the utmx expliot, elf-loader followied by the websrv payloads
 - Start the "Homebrew Launcher" app and then go to "Payload Loader" menu item to load the payloads you want.

## How to update/add payloads
 - Download the payload you want and ftp it to the `/data/homebrew/payload-loaders/payloads` folder
 - The payload will be automatically included the the "Payload Loader" menu item
