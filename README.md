# Présentation du projet
Ce projet a pour vocation à me sevir de terrain de jeu dans mon apprentissage du rendu 3D en temps réel avec Opengl sur le long-terme. J'ai souhaité lui crée une structure logique et qui permettra une mise à echelle simple dans le temps. Il est encore très loin de ce que je désirais, des fonctionnalités sont mal implémentés comme le calcul de la postion du soleil, et donc de la lumière directionelle principal et unique du rendu, par exemple. A l'avenir j'ai pour ambition, qu'il puisse génerer procédurallement des "biomes" avecs des environnements différents, qu'il construira selon certains patternes. 

# Structure du projet
## Dépendences
Le projet comporte en dépendances : glfw une librairie pour le développement en Opengl, glad gérant les pointeurs vers les adresses mémoires des fonctions du drivers, GLM une libraire pour manipuler des classes identiques à ceux présents en GLSL tels que les données de types vecN ou encore matN, et enfin FXTUI qui permet la création de TUI (Textual User Interface) en C++, je l'avais implémenté afin de permetre à l'utilsateur de pouvoir changer le rendu au runtime, en lui donnanant accès différents choix de paramètres, je n'ai malheureusement pas eu le temps de finir d'implementer cette fonctionalité.Enfin, il y a stb_image qui header only et permet de facilement charger des images.

## Mesh module
Le projet est organisé en 5 différents modules : Motion/Shader/Window et enfin Mesh

Après des mois d'apprentissage d'OpenGL à travers des ressources telles que le livre et le site web Learnopengl.com, j'ai pris l'initiative de lancer mon propre projet de rendu 3D. Ce projet comprend diverses fonctionnalités, notamment un système de génération de terrain basé sur une heightMap, la création d'un "skydome" et le calcul de normal map. À terme je souhaiterais que ce projet puisse générer procéduralement des environnements 3D réalistes avec des environnements divers et variés.
Actuellement se projet me sert d’hub d’apprentissage dans lequel j’intègre progressivement de nouveaux aspects dans mes découvertes du développement graphique. Conçu pour être évolutif, permettant ainsi l'ajout facile de nouvelles fonctionnalités, ce projet représente pour moi une exploration continue des concepts de rendu en temps réel, renforçant mes compétences en gestion de projet et en résolution de problèmes, tout en nourrissant ma créativité. Ce projet témoigne de mon engagement envers l'apprentissage continu et ma capacité à relever des défis techniques tout en maintenant une vision à long terme.

#Executer
L'exe se trouve dans le dossier 3DLearningHub, il peut possiblement y avoir un problème d'execution sur certains ordinateur dûes au shader de tessellation. L'executable fonctionne sur 2 pc que j'ai testés mais il comportait une bonne carte graphique.
