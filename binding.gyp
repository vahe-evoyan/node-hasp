{
  "variables": {
  },
  "targets": [
    {
      "target_name": "hasp",
      "sources": [ "hasp.cpp" ],
      "conditions": [
        ["OS=='mac'", {
          "include_dirs": [
            "./libs/hasp/darwin"
          ],
          "libraries": [
            "-L../libs/hasp/darwin",
            "-lhasp_darwin"
          ],
        }],
      ],
    }
  ]
}
