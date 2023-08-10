
# Table of Contents

1.  [Objectif](#org2fd74b0)
2.  [implémentation](#org3085c05)
3.  [exemple d'usage](#org9dfc221)
    1.  [ligne de commande & program_options](#orgc16096d)
    2.  [TCP](#org865b9d2)
    3.  [IPC](#orgda7bb29)
4.  [TODO](#org27255fc)


<a id="org2fd74b0"></a>

# Objectif

Vérifier les performances respectives de TCP et IPC dans le cas d'une communication locale, sachant que:

-   TCP sera extensible à un réseau local contrairement à une mémoire partagée
-   La taille de données standard varie de quelques octets (chaines, nombres) à quelques centaines d'octets (chaine json).

Utiliser boost autant que possible; se baser sur des exemples existants.


<a id="org3085c05"></a>

# implémentation

-   Une paire d'exe client et serveur pour TCP (TCP_srv_sync.exe / TCP_client.exe)
-   et pour IPC (  )
-   chacun permet de définir un pool de données de test côté serveur à transmettre à tout client qui se connecte
-   le client mesure le taux de transfert du message
-   **toujours utiliser release** : perfs mesurées en debug sont très basses
-   l'intégrité du message est vérifiée séparément (todo: TCP)
-   le serveur et le client sont à lancer sur des terminaux différents
-   le serveur doit subsister
-   ici, le client ne connait pas la taille du pool côté serveur. Le pool est ramené en entier


<a id="org9dfc221"></a>

# exemple d'usage


<a id="orgc16096d"></a>

## ligne de commande & program_options

boost::program_options utilisé par tous les programmes.
chaque programme affiche par défaut son aide avec les paramètres accessibles (obligatoires & valeurs par défaut).


<a id="org865b9d2"></a>

## TCP

par défaut, client et serveur se connectent au réseau local (cf option -i)

Sur un terminal:

    .\TCP_srv_sync.exe --port 666 --sizeB 128

Le serveur TCP crée un buffer de 128 octets et le met à disposition sur le port 666.

laisser tourner et basculer sur autre terminal.

    .\TCP_client.exe --port 666 -p 8

le client va se connecter au serveur et ramener le message, ici par paquets de 8 octets.
**!** Les paquets sont recopiés dans l'ordre dans un buffer final, de taille max. 512Mo (TODO)

Le serveur affiche un message quand un client se connecte.
Le client mesure le temps de transfert (µs) et le taux de transfert (Mo/s)

> Termine !
> 
> client: durations (µs):617
> 
> client: bandwidth (Mo/s): 0.197845 (packet size: 8)


<a id="orgda7bb29"></a>

## IPC

Le serveur crée une mémoire partagée dans le système. 
Par défaut, il nettoie la mémoire partagée en fin de programme. De ce fait, on assure ici artificiellement la persistance du serveur. Peut faire autrement?

    .\IPC_server.exe -t coucou --sizeB 128 -v 3

Il affiche le pool (taille et nom), et continue à tourner en attente client.

> parent launched (SM name=coucou)IPC server: pool of size 128 octets / Bytes or 0.00012207 MB / Mo

côté client:

    .\IPC_client.exe -t coucou -v 3

> client launched
> 
> client: shared mem ok, durations (µs):2490 (init sm) / 3 (copy sm payload)
>
> client: region size: 128 bytes / 0 Mo
>
> bandwidth: 40.6901(Mo/s)

le client mesure:

-   le temps d'accès au pool de mémoire partagée (généralement constant peu importe taille du pool)
-   et le temps de copie des données du pool sur un buffer local. Ce dernier temps dépend de la taille du pool
-   Il déduit la vitesse de transfert à partir du second


<a id="org27255fc"></a>

# TODO 

-   gestion exceptions / erreur très perfectible
-   vérification données en TCP: contenu et ordre des paquets
-   TCP: essai asynchrone (perfs, collecte paquets?)
-   TCP: afficher le pool de données comme IPC
-   TCP: gérer la taille max du buffer de réception des messages agrégés si utile.
-   ICP: possible sans serveur?

