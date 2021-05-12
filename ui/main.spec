# -*- mode: python ; coding: utf-8 -*-


block_cipher = None

a = Analysis(['main.pyw'],
             pathex=['/home/avagr/Course/Program/BisimilarityGame/ui'],
             binaries=[],
             datas=[('icons/icon.png', 'icons'), ('icons/icon_small.png', 'icons'),
                    ('stylesheets/colors_dark.xml', 'stylesheets'),
                    ('stylesheets/colors_light.xml', 'stylesheets'),
                    ('stylesheets/table_dark.qml', 'stylesheets'),
                    ('stylesheets/table_light.qml', 'stylesheets'),
                    ('stylesheets/down_arrow.png', 'stylesheets'),
                    ('stylesheets/right_arrow.png', 'stylesheets'),
                    ('instructions.md', '.')],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
          cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          [],
          exclude_binaries=True,
          name='Bisimilarity Game',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          console=True)
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               upx_exclude=[],
               name='main')
