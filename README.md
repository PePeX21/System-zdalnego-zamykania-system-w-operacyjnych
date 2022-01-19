# System-zdalnego-zamykania-system-w-operacyjnych
implementacja serwera w jezyku C znajduje sie w pliku server-thread.c
i klienta w jezyku Java znajduje sie w folderze src w pliku ClientApplication.java

serwer tworzy watek dla nowego polaczenia z klientem i komunikuje sie z nim za pomoca interfejsu gniazd BSD
client w javie do komunikacji wykorzystuje bibloteki java.io.InputStreamReader i java.io.PrintWriter
do stworzenia gniazd komunikacyjnych uzyta jest biblioteka java.net.Socket
  GUI aplikacji stworzone jest z wykorzystaniem biblioteki javax.swing

## Uruchomienie 
plik server-thread.c nalez skompilowac z uzyciem flagi -pthread:
gcc server-thread.c -o server-thread -pthread

klienta mozna uruchomic w kazdym ide dzialajacym z biblioteka Swing
lub uruchamiajac plik wykonywalny sk.jar znajdujacy sie w folderze out

## praca z programem

po uruchomieniu aplikacji klienta pojawi sie okno dialogowe
![Zrzut ekranu (823)](https://user-images.githubusercontent.com/67105405/150037944-dbdb9757-15c0-4f63-b44f-c0dae6164344.png)



nalezy wpisac adres IP serwera i przed zaakceptowaniem upewnic sie
czy serwer jest uruchomiony
![Zrzut ekranu (824)](https://user-images.githubusercontent.com/67105405/150037954-f3e3f5b3-53a5-495b-8250-c02b21fc923b.png)



wybrac z  listy IP komputera do wylaczenia i wybrac przycisk execute
![Zrzut ekranu (825)](https://user-images.githubusercontent.com/67105405/150037993-7da38d11-81c3-4872-b01e-4ef312f01c92.png)



w nowym oknie trzeba wpisac haslo umozliwiajace wykonanie operacji
poprawne haslo: kill
![Zrzut ekranu (826)](https://user-images.githubusercontent.com/67105405/150037994-b03ec2f4-cff8-48c9-b886-fe4e7a515b57.png)




serwer wyswietla komunikaty o podjetych akcjach
![Zrzut ekranu (828)](https://user-images.githubusercontent.com/67105405/150038217-738f14f9-a310-4eb8-bc42-30e04c8ee21f.png)




lista polaczonych klientow jest aktulizowana przy kazdym nowym polaczeniu,
wyjsciu klienta z aplikacji guzikiem exit lub zdalnym wylaczeniu komputera


