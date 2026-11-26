import sys,tempfile,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import control_center as c
class ConfigTests(unittest.TestCase):
 def test_roundtrip_backup(self):
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/'edup-vr-tools.ini';v=c.defaults();v['Tools','EnergyLink']=4;c.save(p,v);self.assertEqual(c.read(p),v);old=p.read_bytes();v['Visual','Overlay']=1;c.save(p,v);self.assertEqual(p.with_suffix('.ini.bak').read_bytes(),old);self.assertEqual(c.read(p),v)
 def test_invalid(self):
  for key,value in [(('Bindings','Jump'),0x11),(('Tools','EnergyLink'),256),(('Bindings','Jump'),0x44),(('Visual','HapticPercent'),101),(('MotionAim','PixelsPerDegree'),0)]:
   v=c.defaults();v[key]=value
   with self.assertRaises(ValueError):c.validate(v)
 def test_legacy_example(self):
  v=c.read(Path(c.__file__).with_name('edup-vr-tools.ini'));self.assertEqual(v['Tools','EnergyLink'],4);self.assertEqual(v['Bindings','Primary'],256)
 def test_tool_preset(self):
  v=c.defaults();p=c.tool_preset(v);self.assertEqual(p['Tools','EnergyLink'],4);self.assertEqual(v['Tools','EnergyLink'],0)
  v['Bindings','Jump']=4
  with self.assertRaises(ValueError):c.tool_preset(v)
 def test_weapon_config(self):
  v=c.defaults();self.assertEqual(v['Movement','WeaponSelect'],0)
  v['Movement','WeaponSelect']=1;v['Weapons','SelectPrimaryWeapon']=2;v['Weapons','SelectSecondaryWeapon']=3
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/'edup-vr-tools.ini';c.save(p,v);self.assertEqual(c.read(p),v)
  v['Weapons','SelectSecondaryWeapon']=256
  with self.assertRaises(ValueError):c.validate(v)
  v['Weapons','SelectSecondaryWeapon']=2
  with self.assertRaises(ValueError):c.validate(v)
 def test_preserve_unknown(self):
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/'edup-vr-tools.ini';p.write_text('[Other]\nFoo=bar\n');c.save(p,c.defaults());self.assertIn('Foo = bar',p.read_text())
if __name__=='__main__':unittest.main()
