hunter_config(bcos-crypto
	VERSION 3.0.0-local
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz"
	SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
)

hunter_config(bcos-framework VERSION 3.0.0-003f26b1
URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/26b98d731ae83e01fa3045107c97acf53e81280d.tar.gz"
SHA1 "dfc5922847132ace44e1304a30a021a2f9d78699"
CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)