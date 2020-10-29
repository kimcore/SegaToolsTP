Expand-Archive -Path minime.zip -DestinationPath .\minimetmp
cd minimetmp
cd minime-master
npm install
npm run build
mkdir ..\..\minime
move -Path ".\*" -Destination "..\..\minime\"