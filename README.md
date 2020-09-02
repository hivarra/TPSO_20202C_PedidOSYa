# tp-2020-2c-Thread-Away

Para hacer que ejecute el programa con la shared linkeada, cerrar todas las terminales y abrir una terminal nueva, poner:
leafpad .bashrc

copiar y pegar la sig linea a lo ultimo: 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Repositorios/tp-2020-2c-Thread-Away/shared/Debug/

con esto, cada vez que ejecuten el programa desde consola, podra encontrar la shared object para ejecutar.
Tener en cuenta que esto se debe hacer tambien en las VM pruebas y en la entrega final.