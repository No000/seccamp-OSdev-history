# seccamp-OSdev-history

セキュリティ・キャンプの開発のヒストリー

# ディレクトリ構成
```
seccamp-OSdev-history
├── ELF_headers
│  └── elf_understand.c
├── README.md
└── UEFIapp
   ├── Bootloader
   ├── History
   │  └── HelloWorld
   │     └── HelloPkg
   │        ├── Hello
   │        │  ├── Hello.c
   │        │  └── Hello.inf
   │        ├── HelloPkg.dec
   │        └── HelloPkg.dsc
   └── Tool
      └── original_firmware
         ├── OVMF.fd
         ├── OVMF_CODE.fd
         └── OVMF_VARS.fd
```
# 特殊な操作

lnコマンドでedk2と開発モジュールをつなぐ
以下はhistoryの中にあるHelloPkgの例
```
cd ~/edk2
ln -s ~/seccamp-OSdev-history/UEFIapp/History/HelloWorld/HelloPkg ./
```

