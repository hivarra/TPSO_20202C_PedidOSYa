<h1>UTN Facultad Regional Buenos Aires</h1>
<h2>Sistemas Operativos</h2>
<h2>Equipo Thread Away</h2>

<h4>Descripción</h4>

Trabajo Práctico (aprobado) "PedidOS Ya!" de la materia Sistemas Operativos, desarrollado en lenguaje C durante el 2do cuatrimestre del año 2020.
Cuenta con diferentes módulos implementados en un entorno distribuido simulando un sistema de pedidos con un manejo adecuado de los recursos.
Los diferentes módulos se conectan entre sí utilizando sockets mediante el protocolo TCP/IP, y dentro de estos utilizamos conceptos de los Sistemas Operativos,
táles como multi-threading, semáforos, sincronización de procesos, planificación de procesos, gestión de memoria y MV, File System, concurrencia, memoria dinamica, entre otros.
El TP se desarrollo en el IDE Eclipse, dentro de un Sistema Operativo Linux, distribución Ubuntu 32 bits.

<h4>Módulos:</h4>
<ul>
  <li>Cliente: proceso encargado de realizar solicitudes de consulta y selección de restaurantes, y creación de pedidos y platos.
  Se creó una API en la cual las distintas solicitudes se ingresan por teclado y son enviadas al módulo al cual se conectó.</li>
  <li>App: proceso planificador de pedidos y asignación de repartidores según algortimos FIFO, SJF sin desalojo y HRRN.</li>
  <li>Comanda: proceso simulador de Memoria Principal con Swap con un esquema de segmentación paginada, en el cual se almacenan los pedidos.</li>
  <li>Restaurante: proceso planificador de platos y asignacion de cocineros según algoritmos FIFO y Round Robin.</li>
  <li>Sindicato: proceso simulador de Filesystem encargado de administrar archivos para el manejo de restaurantes, pedidos, platos y recetas bajo un esquema de asignació de bloques enlazado.</li>
</ul>
