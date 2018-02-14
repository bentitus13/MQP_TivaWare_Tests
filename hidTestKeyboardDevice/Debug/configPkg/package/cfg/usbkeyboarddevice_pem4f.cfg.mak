# invoke SourceDir generated makefile for usbkeyboarddevice.pem4f
usbkeyboarddevice.pem4f: .libraries,usbkeyboarddevice.pem4f
.libraries,usbkeyboarddevice.pem4f: package/cfg/usbkeyboarddevice_pem4f.xdl
	$(MAKE) -f /home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI/src/makefile.libs

clean::
	$(MAKE) -f /home/alex/workspace_v7/usbkeyboarddevice_EK_TM4C123GXL_TI/src/makefile.libs clean

