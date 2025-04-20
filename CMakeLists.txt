cmake_minimum_required(VERSION 3.5)

project(payload-loader)

include_directories(
  source
)

add_executable(payload-loader.elf
  source/main.cpp
)

target_link_libraries(payload-loader.elf
  SceNet
  kernel
)

add_custom_target(package
   DEPENDS payload-loader.elf
   COMMAND rm -rf payload-loader
   COMMAND rm -f payload-loader.zip
   COMMAND mkdir -p payload-loader
   COMMAND cp -R ${PROJECT_SOURCE_DIR}/data/* payload-loader/
   COMMAND cp payload-loader.elf payload-loader
   COMMAND mkdir -p payload-loader/payloads
   COMMAND cp -R ${PROJECT_SOURCE_DIR}/data/sce_sys/icon0.png payload-loader/payloads
   COMMAND curl -L -o 'payload-loader/payloads/etaHen 2.0b.bin' 'https://github.com/etaHEN/etaHEN/releases/download/2.0b/etaHEN-2.0b.bin'
   COMMAND curl -L -o 'payload-loader/payloads/kstuff 1.4.elf' 'https://github.com/EchoStretch/kstuff/releases/download/v1.4/kstuff.elf'
   COMMAND curl -L -o ftpsrv.zip 'https://github.com/ps5-payload-dev/ftpsrv/releases/download/v0.11.3/Payload.zip'
   COMMAND unzip ftpsrv.zip -d payload-loader/payloads
   COMMAND rm ftpsrv.zip
   COMMAND mv payload-loader/payloads/ftpsrv.elf 'payload-loader/payloads/ftpsrv 0.11.3.elf'

   COMMAND zip -r payload-loader.zip payload-loader/
)
