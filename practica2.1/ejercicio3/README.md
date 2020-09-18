# Ejercicio 3

## Compilación

```sh-session
$ make
```

## Ejecución

```sh-session
-- Obtiene el tiempo del servidor:
$ ./time_client 127.0.0.1 2222 t

-- Obtiene la fecha del servidor:
$ ./time_client 127.0.0.1 2222 d

-- Cierra el servidor (ejercicio 2 solo):
$ ./time_client 127.0.0.1 2222 q
```

> **Nota**: este ejercicio tiene dependencia con el [ejercicio 2](../ejercicio2)
y el [ejercicio 6](../ejercicio6).
