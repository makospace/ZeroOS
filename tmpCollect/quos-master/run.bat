rem @call bld.bat
@set dir=qemu
@echo Bengin to run qemu for quos...
@%dir%\qemu.exe -L %dir%\ -hda 1.img -fda quos\QUOS.IMA -m 128 -boot a -localtime