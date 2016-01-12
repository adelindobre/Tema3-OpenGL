Tema 3 Elemente de grafica pe calculator 

Autor: Dobre Grigore Adelin 333CC

Detalii de implementare:

	Am pornit in realizarea temei de la scheletul laboratorului 7.
Am pastrat solul din scheletul de cod, iar sursa de lumina este incarcata din fisierul initial din laborator.
Pentru simularea apei, am ales sa definesc trei unde cu aceeasi amplitudine si lungime de unda, dar cu viteze diferite.
Solul este creat ca un mesh, cu 100X100 vertecsi. Functia de creare a meshului este preluata din lab 5 si adaptata.
view_mattrix este egala matricea de vizualizare a camerei FPS, care initial are aceleasi coordonate cu sursa de lumina.

	Toate proprietatile din main sunt transmise catre vertex shader in notifyDisplayFrame() utilizand "glUniformMatrix4fv()".
Pentru sursa de lumina am folosit matrice_translatie_1 care este inmultita cu functia myTranslate de fiecare data cand sfera este
deplasata pe axa Ox sau Oz, iar pentru simularea apei am folosit matrice_translatie_2. 
Translatarea camerei FPS se face utilizand tastele "u", "j", "h", "k", "i", "y", iar rotatia ei se realizeaza din sageti.

	In vertex shader, pentru calculul valurilor directionale si circulare am ales formula prezentata in documentul aditional de la Nvidia, iar pentru calculul normalei pentru iluminare, am ales metoda aproximarii prezentata in pdf.  Astfel, pentru valoarea dorita fac media aritmetica intre normalele aferente fiecarui triunghi. 

In urma aplicarii transformarilor pentru fiecare dintre cele 3 tipuri de unde si insumarea lor, rezulta vertexul de iesire out_pos cu aceeasi coordonata x si z , dar cu noua valoare obtinuta pentru coordonata y.

	Pentru desenarea separata a valurilor si a sursei de lumina am ales sa trimit catre vertex_shader si o variabila "flag".

	Variabila transmisa catre fragment_shader este un vector de 3 elemente float corespunzatoare celor trei culori, numit "light".

Dispunerea valurilor:
	Am ales ca primul val, cel cu indicele 0, sa fie pe directia axei Ox. Al doilea val este rotit cu 45 de grade(vectorul directie transmis are valoare (0.5, 0.5)). Al treilea val este cel circular iar centrul valului este in afara planului, in coltul din stanga catre z negativ.

		 