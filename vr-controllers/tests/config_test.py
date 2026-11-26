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
 def test_preserve_unknown(self):
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/'edup-vr-tools.ini';p.write_text('[Other]\nFoo=bar\n');c.save(p,c.defaults());self.assertIn('Foo = bar',p.read_text())
if __name__=='__main__':unittest.main()
