# Dispositions de planètes

`PLANET_LAYOUT` accepte trois valeurs :

- `random` génère une disposition et l'enregistre dans `current.txt`.
- `saved` recharge les coordonnées de `PLANET_LAYOUT_FILE`.
- `circle` répartit les planètes en cercle (une seule planète est placée au centre).

Le fichier enregistré contient une paire `x y` par ligne, en pixels. Pour retrouver la même disposition,
gardez le même nombre de planètes et la même taille de fenêtre. Le chemin de `PLANET_LAYOUT_FILE` est relatif
au fichier de configuration utilisé.
