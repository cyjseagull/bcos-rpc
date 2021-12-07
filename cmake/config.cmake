hunter_config(bcos-framework VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/80345086f18586ef1e6122d7bf5eb9688e990472.tar.gz
	SHA1 1fabfc8bb45d3793f5a4789a01f32fb35ec691e2
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)

hunter_config(bcos-tars-protocol
    VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-tars-protocol/archive/6ac99551648d720cdc381112a86c5e378287e8e2.tar.gz
    SHA1 d88a7718b0cf329a19eff2f7fd028e31a94711a3
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON URL_BASE=${URL_BASE}
)

hunter_config(bcos-crypto VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/d91d770edda01502ed5ee0c8fe1efdd35b06d308.tar.gz
	SHA1 d5390f09ce5ffc590dcc80a27b10f1e0c1242087
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=OFF HUNTER_PACKAGE_LOG_INSTALL=ON
)

hunter_config(bcos-boostssl
	VERSION 3.0.0-local
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-boostssl/archive/24b005d5eae39d332f9ca2a4eae12ba3e4a69ed2.tar.gz"
	SHA1 ca925ddbe75d1773a79d2f2744863a936c97f2b8
)