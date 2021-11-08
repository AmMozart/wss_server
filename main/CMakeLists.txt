idf_component_register(SRCS "electric_groups.c" "gpio.c" "dmx.c" "wss_server_example.c" "keep_alive.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/cacert.pem"
                                   "certs/prvtkey.pem")
