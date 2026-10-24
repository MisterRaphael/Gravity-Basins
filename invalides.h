//
// Created by raph on 22/10/2026.
//


//double calculate_mechanical_energy(ObjectInfo *object_info, Planet *planets) {
//    double vx = object_info->x_velocity;
//    double vy = object_info->y_velocity;
//    double kinetic_energy = calculate_kinetic_energy(vx, vy);
//
//    double potential_energy = 0.0;
//    for (int i = 0; i < number_of_planets; i++) {
//        double dx = planets[i].x_position - object_info->x_position;
//        double dy = planets[i].y_position - object_info->y_position;
//        double r_squared = dx * dx + dy * dy + SOFTENING_SQ;
//        potential_energy -= (double)G / sqrt(r_squared);
//    }
//    double meca_energy =kinetic_energy + potential_energy;
//    DrawText(TextFormat("Mechanical Energy : %.0f J", meca_energy), 160, 110, 20, WHITE);
//    return meca_energy;
//}

