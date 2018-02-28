# invoke SourceDir generated makefile for empty_min.pem4f
empty_min.pem4f: .libraries,empty_min.pem4f
.libraries,empty_min.pem4f: package/cfg/empty_min_pem4f.xdl
	$(MAKE) -f C:\Users\Ben\Documents\MQP_Code\MQP_TivaWare_Tests\HID_Test/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Ben\Documents\MQP_Code\MQP_TivaWare_Tests\HID_Test/src/makefile.libs clean

