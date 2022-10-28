# Coreboot Upl and ShimLayer Open

## Enviroment
mkdir CorebootUplShimOpen
cd CorebootUplShimOpen
git clone https://github.com/BinsonWu84/CorebootUplShimOpen.git CorebootUplShim
git clone https://github.com/tianocore/edk2 Edk2
cd Edk2
git submodule update --init
cd ../CorebootUplShim

## Build UefiUpl and ShimLayer
bash CorebootUPLBuild.sh
bash CorebootShimBuild.sh

## Copy to your Coreboot build folder
cp -f Build/ShimLayer.elf $(COREBOOT_WORKSPACE)/build
cp -f Build/UefiUniversalPayload.elf $(COREBOOT_WORKSPACE)/build

## Replace ShimLayer and UefiUpl to your coreboot.rom
cd $(COREBOOT_WORKSPACE)/build
./cbfstool coreboot.rom remove -r COREBOOT -n fallback/payload
./cbfstool coreboot.rom remove -r COREBOOT -n img/UniversalPayload
./cbfstool coreboot.rom add-payload -r COREBOOT -n fallback/payload -f ShimLayer.elf
./cbfstool coreboot.rom add-flat-binary -r COREBOOT -n img/UniversalPayload -f UefiUniversalPayload.elf -l 0x200000 -e 0x100 -c lzma

