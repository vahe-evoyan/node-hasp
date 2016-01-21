{
  "variables": {
  },
  "targets": [
    {
      "target_name": "hasp",
      "sources": [ "src/main.cpp", "src/node_hasp.cpp", "src/hasp.cpp" ],
      "conditions": [
        ["OS=='mac'", {
          "xcode_settings": {
            "OTHER_CFLAGS": ["-std=c++11"],
            "OTHER_CPLUSPLUSFLAGS": ["-std=c++11", "-stdlib=libc++"],
            "OTHER_LDFLAGS": ["-stdlib=libc++"],
            "MACOSX_DEPLOYMENT_TARGET": "10.7"
          },
          "include_dirs": [
            "libs/hasp/darwin"
          ],
          "libraries": [
            "-L../libs/hasp/darwin",
            "-lhasp_darwin",
            "-lcrypto"
          ],
        }],
        ["OS=='linux'", {
          "cflags": [
            "-std=c++11"
          ],
          "include_dirs": [
            "libs/hasp/linux"
          ],
          "libraries": [
            "-L../libs/hasp/linux/<(target_arch)",
            "-lhasp_linux"
          ],
        }],
      ],
    }
  ]
}
