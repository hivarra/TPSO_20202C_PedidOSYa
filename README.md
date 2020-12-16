# tp-2020-2c-Thread-Away

Para hacer que ejecute el programa con la shared linkeada, cerrar todas las terminales y abrir una terminal nueva, 
poner el sig. comando para editar el .bashrc (o usar el editor de preferencia)
leafpad .bashrc

copiar y pegar la sig linea a lo ultimo: 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Repositorios/tp-2020-2c-Thread-Away/shared/Debug/

Con esto, cada vez que ejecuten el programa desde consola, se podra encontrar la shared object para ejecutar.
Tener en cuenta que esto se debe hacer tambien en las VM pruebas y en la entrega final (usando otro editor).

En cuanto a las entregas finales, upload al repo los archivos makefile, objects.mk y sources.mk
de cada proyecto, a fin de poder compilar en otras VMs.

<h1>Pasos para bailar salsa AHRE INSTALAR TODO:</h1>

#PASO 0 <br>
- vi .bashrc
- Ir al final de todo y presionar la tecla "i" para que permita ingresar texto
- Pegar el siguiente texto: <br>
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Repositorios/tp-2020-2c-Thread-Away/shared/Debug/
- Escape (para cancelar el INSERTAR)
- :wq + enter

#PASO 1 <br>
mkdir Repositorios

#PASO 2 <br>
cd Repositorios

#PASO 3 <br>
vi deploy.sh
(Copiar el contenido de este link: https://github.com/sisoputnfrba/tp-2020-2c-Thread-Away/blob/master/deploy.sh)
Pegar con click derecho

#PASO 4 <br>
Escape (para cancelar el INSERTAR)
:wq + enter

#PASO 5 <br>
chmod 777 deploy.sh

#PASO 6 <br>
./deploy.sh -t=/home/utnso/Repositorios -d=shared -p=app -p=comanda -p=cliente -p=restaurante -p=sindicato tp-2020-2c-Thread-Away
