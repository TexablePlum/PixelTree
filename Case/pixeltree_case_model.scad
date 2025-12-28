// ============================================================================
// PixelTree Controller Enclosure
// For: DC-DC Converter + ESP32 + Pixel Booster
// ============================================================================

// --- PARAMETERS (adjust if needed) ---
wall = 2.5;              // Wall thickness
tolerance = 0.3;         // Printer tolerance

// Internal dimensions (user specified)
inner_length = 75;       // 15mm socket + 58mm converter + 2mm margin
inner_width = 70;        // as specified
inner_height = 30;       // as specified

// Calculated outer dimensions
outer_length = inner_length + 2*wall;
outer_width = inner_width + 2*wall;
outer_height = inner_height + wall;  // bottom wall only

// Cable holes
dc_socket_hole = 11;     // DC 2.1/5.5 panel mount socket (M11 thread)
led_cable_hole = 4.5;    // 3x LED wires (1.4mm each) - minimal size

// PRINT COLOR: Black filament (PLA/PETG)

// Ventilation
vent_slot_width = 2;
vent_slot_length = 15;
vent_spacing = 5;

// Lid parameters
lid_lip = 2;             // Lip that goes inside box
lid_height = 5;          // Total lid height

// Corner radius
corner_r = 3;

// --- MODULES ---

module rounded_box(l, w, h, r) {
    hull() {
        for (x = [r, l-r]) {
            for (y = [r, w-r]) {
                translate([x, y, 0])
                    cylinder(h=h, r=r, $fn=32);
            }
        }
    }
}

module base() {
    difference() {
        // Outer shell
        rounded_box(outer_length, outer_width, outer_height, corner_r);
        
        // Inner cavity
        translate([wall, wall, wall])
            rounded_box(inner_length, inner_width, inner_height + 1, corner_r - wall/2);
        
        // DC socket hole - input side (24V DC in) - panel mount socket
        translate([-1, outer_width/2, outer_height/2])
            rotate([0, 90, 0])
                cylinder(d=dc_socket_hole, h=wall+2, $fn=32);
        
        // LED cable hole - output side (to LED strip - 3 wires)
        translate([outer_length - wall - 1, outer_width/2, outer_height/2])
            rotate([0, 90, 0])
                cylinder(d=led_cable_hole, h=wall+2, $fn=32);
        
        // Ventilation slots - symmetric on both long sides
        vent_count = 3;
        total_vents_width = vent_count * vent_slot_length + (vent_count - 1) * vent_spacing;
        vent_start_x = (outer_length - total_vents_width) / 2;
        
        for (i = [0:vent_count-1]) {
            // Front side vents
            translate([vent_start_x + i*(vent_slot_length + vent_spacing), -1, outer_height - 8])
                cube([vent_slot_length, wall+2, vent_slot_width]);
            // Back side vents
            translate([vent_start_x + i*(vent_slot_length + vent_spacing), outer_width - wall - 1, outer_height - 8])
                cube([vent_slot_length, wall+2, vent_slot_width]);
        }
    }
}

module lid() {
    // Simple friction-fit lid (use glue or welding for permanent attachment)
    union() {
        // Main lid
        rounded_box(outer_length, outer_width, lid_height - lid_lip, corner_r);
        
        // Inner lip that fits inside box
        translate([wall + tolerance, wall + tolerance, lid_height - lid_lip])
            rounded_box(inner_length - 2*tolerance, inner_width - 2*tolerance, lid_lip, corner_r - wall/2);
    }
}

// --- RENDER ---
// Uncomment what you want to render/export:

// Both parts side by side (for preview)
base();
translate([outer_length + 10, 0, 0])
    lid();

// Just base:
// base();

// Just lid:
// lid();

// Lid flipped for printing:
// translate([0, 0, lid_height]) rotate([180, 0, 0]) lid();
