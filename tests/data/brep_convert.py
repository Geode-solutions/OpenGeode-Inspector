import opengeode
import opengeode_geosciences
import opengeode_geosciencesio

brep = opengeode_geosciences.load_structural_model("model_A1_valid.ml")
opengeode_geosciences.save_structural_model(brep,"model_A1_valid.og_brep")

brep2 = opengeode_geosciences.load_structural_model("model_A1.ml")
opengeode_geosciences.save_structural_model(brep2,"model_A1.og_brep")
