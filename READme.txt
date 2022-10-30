Documentation de la Version "Historique Ephémère"

**Les commandes à la disposition des clients**

1.  La déconnexion

Lorsqu’un client s’est connecté sur le serveur, il a la possibilité de se 
déconnecter à tout moment. Le client doit entrer la commande “quit” pour mettre fin à 
sa connexion avec le serveur. Cette fonctionnalité est directement géré côté client.  
Un serveur étant censé être connecté 24/7, nous avons décidé de ne pas 
implémenter cette fonctionnalité pour le serveur.

2. Le message privé

Pour envoyer un message privé à une personne X, un client Y peut écrire la commande "Private to X : [message]".
Seul le client X recevra alors le message provenant de X. Ce message sera affiché dans le terminal du serveur mais ne sera pas
sauvegardé dans l'historique.

3. Créer un groupe

Pour créer un groupe avec un client X et Y, un client Z peut rentrer une commande de forme "Group [nom du groupe] : X, Y, Z".
Le nombre de membre maximal dans un groupe est de 10 personnes. 

4. Envoyer un message dans un groupe

Pour envoyer un message dans un groupe, un client peut écrire la commande "To [nom du groupe] : [message]".
Seules les personnes du groupe recevront le message.

5. Ajouter un membre dans un groupe

Pour ajouter le client X à un groupe, un client quelconque peut écrire la commande "Add to [nom du groupe] : X".
On ne peut ajouter qu'une personne à la fois dans un groupe.

6. Retirer un membre d'un groupe

Pour retirer le client X à un groupe, un client quelconque peut écrire la commande "Remove from [nom du groupe] : X".
On ne peut retirer qu'une personne à la fois d'un groupe.

Les debuggers

Afin de debugger le travail sur les groupes et les connexions, nous pouvons demander au serveur d'afficher la liste des 
clients et des groupes. Dans n'importe quel terminal client connecté au serveur, on peut taper "Voir groupes" pour voir dans le terminal
serveur les différents groupes et ses membres. La commande "Voir clients" permet de voir les clients connectés.

**Les logs**

De manière générale, le terminal serveur affiche chaque action réalisé par les clients connectés :
- Les connexions/déconnexions
- Les messages à tout le monde
- Les messages privés
- Les messages groupés
- Les créations de groupe
- Les ajout/suppression de membre dans un groupe

Cela permet de suivre le bon déroulé du code.

**Historique de discussion**

Pour permettre à un client qui n’était pas connecté de récupérer le fil de la discussion 
nous avons implémenté un historique. Les fonctions services et la structure 
ont été codées dans un fichier C à part (history.c). L’historique est directement géré et 
gardé en mémoire côté serveur.
Les données enregistrées dans l’historique sont seulement issues des discussions 
publiques et des actions de connexion/déconnexion. 
L’historique s’affiche dés qu’un nouveau client se connecte au Socket. Un 
aperçu des messages qui ont été échangé pendant son absence lui est directement envoyé.
Notre historique est éphémère. Si le serveur se coupe, l'historique est perdu.

**Axes d'améliorations de notre code**

L’un des axes d’amélioration envisageable aurait été de produire un 
historique personnel pour chaque client. Dans ce cas précis on aurait pu prendre en 
compte les messages privés et les messages de groupe dans l’historique.  
Stocker les différents historiques sous forme de fichier txt aurait permis d'avoir un POC
viable à déployer.
Des mesures de sécurités seraient aussi à ajouter. Aujourd'hui, chaque client peut ajouter ou retirer des membres dans 
n'importe quel groupe. Chaque groupe est donc entièrement publique.

**Lancer le serveur et les clients**

Pour compiler les fichiers serveurs (depuis le dossier Serveur) : gcc server2.c history.c -o serveur
Pour le lancer : ./serveur

Pour compiler les fichiers clients (depuis le dossier Client) : gcc server.c -o client
Pour le lancer : ./client [IP du serveur] [pseudo du client]