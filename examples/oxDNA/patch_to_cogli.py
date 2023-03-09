import baggianalysis as ba
import numpy as np

PATCH_DELTA = 0.1
PATCH_THETA = 0.6

patches = [np.array([0.000000, 0.000000, 0.500000]),
           np.array([-0.235702, 0.408248, -0.166667]),
           np.array([-0.235702, -0.408248, -0.166667]),
           np.array([0.471405,  0.000000, -0.166667])
           ]

def add_patches(p, orientation_matrix):
    for patch in patches:
        p.add_orientation_vector(orientation_matrix @ patch)

parser = ba.OxDNAParser()
parser.set_orientation_inserter(add_patches)
system = parser.make_system("last_conf.dat")

def converter(p):
    ret = "%lf %lf %lf @ 0.5 C[red] M " % (p.position[0], p.position[1], p.position[2])

    for patch in p.orientation_vectors:
        patch_pos = patch * (1.0 + PATCH_DELTA)
        ret += "%lf %lf %lf %lf C[blue] " % (patch_pos[0], patch_pos[1], patch_pos[2], PATCH_THETA)

    return ret

exporter = ba.Cogli1Exporter(converter, True)
exporter.write(system, "last_conf.dat.mgl")
