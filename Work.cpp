#include "Work.h"

int Work::getId() const {
    return id;
}

void Work::execute() {
    executeFunction();    
}
