"""ED-UP VR Control Center — local configuration only, no game input."""
import configparser
import json
import os
from pathlib import Path
import shutil
import tempfile

# Section, key, French action, physical scan code / mouse code.
ACTIONS = [
 ('Bindings','Forward','Avancer',0x11),('Bindings','Back','Reculer',0x1f),('Bindings','Left','Pas à gauche',0x1e),('Bindings','Right','Pas à droite',0x20),
 ('Bindings','Jump','Sauter',0x39),('Bindings','Interact','Interagir',0x12),('Bindings','Crouch','Accroupissement',0x2e),('Bindings','Sprint','Courir',0x2a),
 ('Bindings','CycleTool','Changer d’équipement',0x10),('Bindings','Reload','Recharger',0x13),('Bindings','Primary','Action principale',256),('Bindings','Secondary','Action secondaire',257),
 ('Tools','EnergyLink','Transfert d’énergie',0),('Tools','ProfileAnalyser','Analyseur de profil',0),('Tools','SuitTool','Outil de combinaison',0),('Tools','Holster','Ranger équipement',0),('Tools','Flashlight','Lampe',0),('Tools','ToolMode','Mode de l’outil',0),
 ('Weapons','SelectPrimaryWeapon','Sortir arme principale',0),('Weapons','SelectSecondaryWeapon','Sortir arme secondaire',0)]
OPTIONS = [('MotionAim','Enabled','Visée par mouvement',1,0,1),('MotionAim','TwoHandEnabled','Visée à deux mains (expérimentale)',0,0,1),('Movement','LeftHanded','Profil gaucher (inverse les manettes)',0,0,1),('MotionAim','PixelsPerDegree','Sensibilité main (px/degré)',10,1,30),('MotionAim','InvertY','Inverser axe vertical',0,0,1),('Movement','SnapMode','Rotation : 0 fluide, 1/2/3 = 15/30/45° indicatifs',0,0,3),('Movement','SnapPixelsPerDegree','Calibration rotation (px/degré)',10,1,30),('Movement','ExtraLookDeadzonePercent','Zone morte supplémentaire visée (%)',0,0,40),('Movement','LookPixelsPerSecond','Sensibilité stick (px/s)',900,0,3000),('Movement','ToolLayer','Couche outils (grip non dominant)',1,0,1),('Movement','EquipmentGestures','Gestes équipements (grip soutien puis grip dominant)',0,0,1),('Movement','EquipmentScalePercent','Échelle zones corporelles (%)',100,70,130),('Movement','WristMenu','Menu radial au poignet de soutien (option)',0,0,1),('Movement','RadialMenu','Menu radial (grip soutien + stick dominant + A)',0,0,1),('Movement','WeaponSelect','Sélection armes : grip soutien + stick dominant',0,0,1),('Movement','StrictTools','Sélection outils sécurisée (retour au neutre)',0,0,1),('Visual','ObjectLab','Laboratoire 3D (exclusif, sans commandes jeu)',0,0,1),('Visual','Hands','Canal visualiseur local',0,0,1),('Visual','Overlay','Mains dans le casque',0,0,1),('Visual','Laser','Pointeur visuel',0,0,1),('Visual','Haptics','Vibrations',0,0,1),('Visual','HapticPercent','Intensité vibration (%)',25,0,100)]
KEYS = {0:'Désactivé',256:'Souris gauche',257:'Souris droite',0x01:'Échap',0x0e:'Retour arrière',0x0f:'Tab',0x1c:'Entrée',0x1d:'Ctrl gauche',0x2a:'Maj gauche',0x36:'Maj droite',0x38:'Alt gauche',0x39:'Espace'}
for start, labels in [(2,'1234567890'),(0x10,'AZERTYUIOP'),(0x1e,'QSDFGHJKLM'),(0x2c,'WXCVBN')]:
 for offset,label in enumerate(labels): KEYS[start+offset]=label
for i in range(1,13):
 if i!=10: KEYS[0x3a+i if i<=10 else 0x57+i-11]='F'+str(i)
# F10 reserved by ED-UP; all other uncommon codes remain readable for existing files.
def label(v): return f'{KEYS.get(v,"Code physique")} [0x{v:02X}]'
def defaults():
 return {(s,k):v for s,k,_,v in ACTIONS}|{(s,k):v for s,k,_,v,_,_ in OPTIONS}
def tool_preset(values):
 # Explicit example, not detected game bindings. Validate before altering the form.
 proposed=dict(values)
 for key,code in zip(('EnergyLink','ProfileAnalyser','SuitTool','Holster','Flashlight','ToolMode'),(4,5,6,0x23,0x14,0x32)):
  proposed['Tools',key]=code
 return validate(proposed)

def validate(values):
 seen={}
 for s,k,title,_ in ACTIONS:
  v=values[s,k]
  if type(v) is not int or not (0<=v<=127 or (s=='Bindings' and v in (256,257))) or v==0x44: raise ValueError(f'{title} : code invalide ou F10 réservé.')
  if v and v in seen: raise ValueError(f'Affectation en double : {title} / {seen[v]}')
  if v: seen[v]=title
 for s,k,title,_,lo,hi in OPTIONS:
  v=values[s,k]
  if type(v) is not int or not lo<=v<=hi: raise ValueError(f'{title} : valeur attendue entre {lo} et {hi}.')
 return values

def read(path):
 values=defaults(); cfg=configparser.ConfigParser(interpolation=None); cfg.optionxform=str.lower
 if path.exists():
  with path.open('r',encoding='utf-8-sig') as f: cfg.read_file(f)
  sections={s.lower():s for s in cfg.sections()}
  for s,k,_,_ in ACTIONS:
   if s.lower() in sections and cfg.has_option(sections[s.lower()],k): values[s,k]=int(cfg.get(sections[s.lower()],k),0)
  for s,k,_,_,_,_ in OPTIONS:
   if s.lower() in sections and cfg.has_option(sections[s.lower()],k): values[s,k]=int(cfg.get(sections[s.lower()],k),10)
 return validate(values)

def save(path,values):
 validate(values)
 if path.name.lower()!='edup-vr-tools.ini': raise ValueError('Nom du fichier INI incorrect.')
 if not path.parent.is_dir(): raise ValueError('Dossier inexistant.')
 # Preserve unrelated INI settings. ConfigParser rewrites formatting and comments.
 cfg=configparser.ConfigParser(interpolation=None);cfg.optionxform=str
 if path.exists():
  with path.open('r',encoding='utf-8-sig') as f: cfg.read_file(f)
 for s,k,v in [(s,k,f'0x{values[s,k]:X}') for s,k,_,_ in ACTIONS]+[(s,k,str(values[s,k])) for s,k,_,_,_,_ in OPTIONS]:
  section=next((x for x in cfg.sections() if x.lower()==s.lower()),s)
  if not cfg.has_section(section): cfg.add_section(section)
  for old in list(cfg[section]):
   if old.lower()==k.lower() and old!=k: cfg.remove_option(section,old)
  cfg.set(section,k,v)
 fd,tmp=tempfile.mkstemp(prefix='.edup-',suffix='.ini',dir=path.parent)
 try:
  with os.fdopen(fd,'w',encoding='utf-8',newline='\n') as f: cfg.write(f);f.flush();os.fsync(f.fileno())
  if path.exists(): shutil.copy2(path,path.with_suffix('.ini.bak'))
  os.replace(tmp,path)
 finally:
  if os.path.exists(tmp): os.unlink(tmp)

def runtime_family(manifest, library=''):
 # Identify known filename conventions only; do not infer headset connectivity.
 names=[str(x).replace('\\','/').rsplit('/',1)[-1].lower() for x in (manifest,library)]
 if any(n in ('steamxr_win64.json','vrclient_x64.dll','vrclient.dll') for n in names): return 'SteamVR'
 if any(n in ('oculus_openxr_64.json','oculus_openxr_32.json','oculus_openxr_64.dll','oculus_openxr_32.dll') for n in names): return 'Meta / Oculus'
 if any(n in ('virtualdesktop-openxr.json','vdxr.json','vdxr.dll') for n in names): return 'Virtual Desktop / VDXR'
 return 'Autre / inconnu'

def inspect_runtime(manifest, source):
 report={'source':source,'manifest':str(manifest or ''),'provider':'Non détecté','manifest_ok':False,'library_ok':False,'library':'','error':''}
 if not manifest: return report
 report['provider']=runtime_family(manifest)
 try:
  p=Path(manifest)
  with p.open('rb') as f: raw=f.read(1024*1024+1)
  if len(raw)>1024*1024: raise ValueError('Manifest trop volumineux (limite 1 Mio)')
  data=json.loads(raw.decode('utf-8-sig'))
  if not isinstance(data,dict) or not isinstance(data.get('runtime'),dict): raise ValueError('Structure du manifest invalide')
  lib=data['runtime']['library_path']
  if not isinstance(lib,str) or not lib.strip() or not isinstance(data.get('file_format_version'),str) or not data['file_format_version'].strip(): raise ValueError('Manifest OpenXR invalide')
  report['manifest_ok']=True
  report['provider']=runtime_family(manifest,lib)
  target=Path(lib)
  if not target.is_absolute(): target=p.parent/target
  report['library']=str(target);report['library_ok']=target.is_file()
 except (OSError,ValueError,KeyError,TypeError) as e:report['error']=str(e)
 return report

def active_runtime():
 override=os.environ.get('XR_RUNTIME_JSON')
 if override:return inspect_runtime(override,'XR_RUNTIME_JSON')
 if os.name!='nt':return inspect_runtime(None,'Windows uniquement')
 import winreg
 try:
  with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,r'SOFTWARE\Khronos\OpenXR\1',0,winreg.KEY_READ|winreg.KEY_WOW64_64KEY) as k:
   value,_=winreg.QueryValueEx(k,'ActiveRuntime')
  return inspect_runtime(value,'HKLM 64 bits')
 except OSError as e:
  report=inspect_runtime(None,'HKLM 64 bits');report['error']=str(e);return report

def main():
 import tkinter as tk
 from tkinter import ttk,filedialog,messagebox
 root=tk.Tk();root.title('ED-UP VR Control Center v0.9.17 — Philou Cyberpunk');root.geometry('840x760');root.minsize(720,640)
 style=ttk.Style(root);style.theme_use('clam');style.configure('Header.TLabel',font=('Segoe UI',18,'bold'));style.configure('TButton',padding=6)
 body=ttk.Frame(root,padding=16);body.pack(fill='both',expand=True)
 ttk.Label(body,text='ED-UP VR CONTROL CENTER',style='Header.TLabel').pack(anchor='w');ttk.Label(body,text='Odyssey à pied • Quest / Valve Index • OpenXR • Configuration locale').pack(anchor='w',pady=(0,12))
 path=tk.StringVar(value='');status=tk.StringVar(value='Sélectionnez le dossier du jeu. Aucun fichier modifié.');row=ttk.Frame(body);row.pack(fill='x');ttk.Entry(row,textvariable=path,state='readonly').pack(side='left',fill='x',expand=True)
 tabs=ttk.Notebook(body);tabs.pack(fill='both',expand=True,pady=12);vars={}; comboboxes={}
 for title,section in [('Déplacements / actions','Bindings'),('Outils / combinaison','Tools'),('Armes','Weapons')]:
  tab=ttk.Frame(tabs,padding=14);tabs.add(tab,text=title)
  ttk.Label(tab,text='Codes physiques AZERTY ; reproduire les mêmes liaisons dans le jeu.').grid(row=0,column=0,columnspan=2,sticky='w',pady=(0,8))
  tab.columnconfigure(1,weight=1)
  for i,(s,k,t,v) in enumerate([a for a in ACTIONS if a[0]==section],1):
   ttk.Label(tab,text=t).grid(row=i,column=0,sticky='w',padx=(0,20),pady=3)
   var=tk.StringVar(value=label(v));vars[s,k]=var;choices=[label(x) for x in sorted(KEYS) if section=='Bindings' or x<256]
   c=ttk.Combobox(tab,textvariable=var,values=choices,state='readonly');c.grid(row=i,column=1,sticky='ew',pady=3);comboboxes[s,k]=c
 settings=ttk.Frame(tabs,padding=14);tabs.add(settings,text='Sensibilité / affichage');settings.columnconfigure(1,weight=1)
 comfort=ttk.Frame(tabs,padding=14);tabs.add(comfort,text='Confort VR');comfort.columnconfigure(1,weight=1)
 for i,(s,k,t,v,lo,hi) in enumerate(OPTIONS):
  pane=comfort if s=='Movement' else settings
  var=tk.IntVar(value=v);vars[s,k]=var
  if hi==1: ttk.Checkbutton(pane,text=t,variable=var).grid(row=i,column=0,columnspan=2,sticky='w',pady=5)
  else:
   ttk.Label(pane,text=t).grid(row=i,column=0,sticky='w',pady=5);ttk.Spinbox(pane,from_=lo,to=hi,textvariable=var,width=10).grid(row=i,column=1,sticky='w')
 diag=ttk.Frame(tabs,padding=14);tabs.add(diag,text='Diagnostic / aide');result=tk.StringVar(value='Aucun dossier sélectionné.');ttk.Label(diag,textvariable=result,wraplength=680,justify='left').pack(anchor='w',pady=(0,12));ttk.Label(diag,text='Ce panneau ne communique pas directement avec le casque.\nPour calibrer : en jeu à pied, F10 puis contrôleurs au neutre ;\nmaintenir le grip dominant pour recaler la visée relative.\nPour deux mains : grip dominant, puis grip non dominant.\nFermer le jeu avant de modifier le fichier. Redémarrage obligatoire.\nEDUP_VR_ONFOOT=1 reste nécessaire dans l’environnement du jeu.\nLes variables EDUP_VR_HANDS/OVERLAY/LASER/HAPTICS à 0 ou 1\nont priorité sur les choix de ce panneau.',justify='left').pack(anchor='w')
 runtime_tab=ttk.Frame(tabs,padding=14);tabs.add(runtime_tab,text='SteamVR / OpenXR')
 runtime_text=tk.StringVar(value='Cliquez sur Détecter le runtime.')
 ttk.Label(runtime_tab,textvariable=runtime_text,wraplength=680,justify='left').pack(anchor='w',pady=(0,12))
 def refresh_runtime():
  try:
   r=active_runtime()
   runtime_text.set('Runtime : '+r['provider']+'\nSource : '+r['source']+'\nManifest : '+(r['manifest'] or 'absent')+'\nManifest valide : '+('oui' if r['manifest_ok'] else 'non')+'\nBibliothèque : '+(r['library'] or 'inconnue')+'\nFichier présent : '+('oui' if r['library_ok'] else 'non')+('\nErreur : '+r['error'] if r['error'] else ''))
  except Exception as e:runtime_text.set('Diagnostic indisponible : '+str(e))
 ttk.Button(runtime_tab,text='Détecter le runtime',command=refresh_runtime).pack(anchor='w',pady=8)
 ttk.Label(runtime_tab,text='SteamVR : ouvrir ses paramètres OpenXR et choisir SteamVR comme runtime si souhaité.\nMeta : utiliser les réglages OpenXR de Meta Quest Link.\nRelancer le jeu après changement. Aucun changement automatique.\nLe diagnostic ne démarre pas une session XR et ne valide pas les manettes.',wraplength=680,justify='left').pack(anchor='w')
 def apply(values):
  for s,k,_,_ in ACTIONS:
   val=label(values[s,k]);c=comboboxes[s,k];choices=list(c['values']);
   if val not in choices: c.configure(values=choices+[val])
   vars[s,k].set(val)
  for s,k,_,_,_,_ in OPTIONS: vars[s,k].set(values[s,k])
 def collect():
  vals={(s,k):int(vars[s,k].get().rsplit('[0x',1)[1][:-1],16) for s,k,_,_ in ACTIONS}
  vals.update({(s,k):vars[s,k].get() for s,k,_,_,_,_ in OPTIONS});return validate(vals)
 def preset_tools():
  try:
   if not messagebox.askyesno('Préréglage Odyssey','Appliquer les six touches exemples 3, 4, 5, H, T, M ? Reproduisez ces liaisons dans le jeu. Aucun fichier ne sera écrit avant Enregistrer.'):return
   apply(tool_preset(collect()));status.set('Préréglage outils chargé, non enregistré.');check()
  except Exception as e:messagebox.showerror('Préréglage impossible',str(e))
 def browse():
  folder=filedialog.askdirectory(title='Dossier contenant EliteDangerous64.exe')
  if not folder:return
  d=Path(folder);p=d/'edup-vr-tools.ini'
  if not (d/'EliteDangerous64.exe').is_file(): messagebox.showerror('Dossier invalide','EliteDangerous64.exe est absent.');return
  try: v=read(p)
  except Exception as e: messagebox.showerror('INI invalide',str(e));return
  path.set(str(p));apply(v);status.set('Configuration chargée.' if p.exists() else 'Nouveau profil : outils désactivés par défaut.');check()
 def check():
  try:
   collect();p=Path(path.get()) if path.get() else None
   result.set('Réglages valides.\n'+(f'Fichier : {p}\nINI présent : {p.exists()}\nSauvegarde : {p.with_suffix(".ini.bak").exists()}' if p else 'Aucun dossier sélectionné.')+'\nCasque et DLL : non testés par cette interface.')
  except Exception as e: result.set('Configuration invalide : '+str(e))
 def write():
  try:
   if not path.get():raise ValueError('Sélectionnez d’abord le dossier du jeu.')
   if not messagebox.askyesno('Enregistrement','Le jeu est-il fermé ? Les réglages seront lus au prochain démarrage.'):return
   save(Path(path.get()),collect());status.set('Profil enregistré ; ancienne version sauvegardée si présente.');check()
  except Exception as e:messagebox.showerror('Enregistrement impossible',str(e))
 ttk.Button(row,text='Choisir dossier…',command=browse).pack(side='left',padx=(8,0))
 buttons=ttk.Frame(body);buttons.pack(fill='x');ttk.Button(buttons,text='Vérifier',command=check).pack(side='left');ttk.Button(buttons,text='Préréglage outils',command=preset_tools).pack(side='left',padx=8);ttk.Button(buttons,text='Réinitialiser le formulaire',command=lambda:(apply(defaults()),status.set('Valeurs par défaut non enregistrées.'))).pack(side='left',padx=8);ttk.Button(buttons,text='Enregistrer',command=write).pack(side='right');ttk.Label(body,textvariable=status,wraplength=730).pack(anchor='w',pady=(10,0));root.mainloop()
if __name__=='__main__':main()
