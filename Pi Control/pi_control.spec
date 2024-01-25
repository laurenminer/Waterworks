# -*- mode: python -*-

block_cipher = None


a = Analysis(['pi_control.py'],
             pathex=['/Users/stephen/Desktop/Pi Control'],
             binaries=[],
             datas=[],
             hiddenimports=['packaging', 'packaging', 'packaging.version', 'packaging.specifiers', 'packaging.requirements', 'six'],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='pi_control',
          debug=False,
          strip=False,
          upx=False,
          console=True )
