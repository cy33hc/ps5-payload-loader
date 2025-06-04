cmake_minimum_required(VERSION 3.5)

project(payload-loader)

if(DEFINED ENV{JAR_LOADER})
  add_definitions(-DJAR_LOADER=1)
  message("BDJ Enabled")
else()
  message("BDJ Disabled")
endif()

include_directories(
  source
)

add_executable(payload-loader.elf
  source/main.cpp
)

target_link_libraries(payload-loader.elf
  kernel_sys
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
   COMMAND curl -L -o 'payload-loader/payloads/etaHen 2.1b.bin' 'https://github.com/etaHEN/etaHEN/releases/download/2.1B/etaHEN.bin'
   COMMAND curl -L -o 'payload-loader/payloads/kstuff 1.5.elf' 'https://github.com/EchoStretch/kstuff/releases/download/v1.5/kstuff.elf'
   COMMAND curl -L -o 'payload-loader/payloads/ezremote-dpi 1.02.elf' 'https://github.com/cy33hc/ps5-ezremote-dpi/releases/download/1.02/ezremote-dpi.elf'
   COMMAND curl -L -o ftpsrv.zip 'https://github.com/ps5-payload-dev/ftpsrv/releases/download/v0.12/Payload.zip'
   COMMAND unzip ftpsrv.zip -d payload-loader/payloads
   COMMAND rm ftpsrv.zip
   COMMAND mv payload-loader/payloads/ftpsrv.elf 'payload-loader/payloads/ftpsrv 0.20.elf'
   COMMAND curl -L -o kstuff-toggle.zip 'https://github.com/EchoStretch/kstuff-toggle/releases/download/0.2/Kstuff-Toggle.zip'
   COMMAND unzip kstuff-toggle.zip -d payload-loader/payloads
   COMMAND rm kstuff-toggle.zip
   COMMAND mv payload-loader/payloads/kstuff-toggle.elf 'payload-loader/payloads/kstuff-toggle 0.2.elf'
   COMMAND rm payload-loader/payloads/homebrew.js

   COMMAND zip -r payload-loader.zip payload-loader/
)

if(DEFINED ENV{JAR_LOADER})
  add_custom_target(jar_loader
    DEPENDS payload-loader.elf
    COMMAND mv payload-loader.elf payload-loader-for-ps5-jar-loader.elf
  )
endif()