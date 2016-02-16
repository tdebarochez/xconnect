
format:
	clang-format -i $(shell find {FDSConnectServerX,XConnect,XConnectClient,XConnectLib,XConnectServer} -type f -name "*.c*")

cleanup:
	$(RM) $(shell find . -type f \( -name ".DS_Store" -o -name "Thumbs.db" \))

.PHONY: format
