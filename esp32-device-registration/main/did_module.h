#ifndef DID_MODULE_H
#define DID_MODULE_H

void did_generate(void);
void did_register_start(void);
char *did_get(void);
char *did_doc_get(void);
void did_destroy_key();

#endif // DID_MODULE_H