hunter_config(bcos-framework VERSION 3.0.1-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/fd6838236e1f952e46720b9c08d310f9055fee0a.tar.gz
	SHA1 85b16b418c8b2d799ef44ba5d0442779c1fd5d4d
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-crypto
	VERSION 3.0.0-local-43df7524
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/255002b047b359a45c953d1dab29efd2ff6eb080.tar.gz"
	SHA1 4d02de20be1f9bf79d762c5b8686368286504e07
)

hunter_config(bcos-boostssl
	VERSION 3.0.0-local
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-boostssl/archive/1b972a6734ef49ac4ca56184d31fe54a08a97e82.tar.gz"
	SHA1 6d35d940eacb7f41db779fb8182cbebf50535574
)
