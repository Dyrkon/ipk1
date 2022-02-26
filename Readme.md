# IPK projekt 1

Projekt v jazyce C který implementuje jednodychý server poskytující informace o jeho hostiteli.

## Příprava a instalace

Soubor buď rozbalíte po stažení ze školního serveru, nebo v případě selhání pullnutím [tagu](https://github.com/Dyrkon/ipk1) z githubu.

### Prekvizity

Co budete potřebovat pro spuštění projektu:

* Linux
* make
* gcc
* curl

### Instalace

Projekt připravíte ke spuštění následujícím způsobem:

```
$ # přesuňte se do složky s projektem
$ make
$ # tímto byl vytvořen binární soubor hinfosvc
```

## Spuštění

Projekt můžete spustit a testovat následovně:

```
$ ./hinfosvc 5000 & # port můžete zvolit libovolně
```
Nyní jste spustili server a můžete mu posílat dotazy několika způsoby:
```
$ curl http://localhost:5000/hostname
$ curl http://localhost:5000/cpu-name
$ curl http://localhost:5000/load
```
Alternativně můžete server dotazovat z libovolného prohlížeče zadáním adresy ve tvary:
```
http://localhost:5000/hostname
```

## Zdroje a poznámky TODO

*
