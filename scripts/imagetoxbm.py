from PIL import Image

def convert_to_xbm(input_image_path, output_xbm_path):
    image = Image.open(input_image_path).convert('1')  # Abre a imagem e converte para preto e branco
    image.save(output_xbm_path)

input_image_path = 'logo.png'
output_xbm_path = 'imagem_convertida.xbm'
convert_to_xbm(input_image_path, output_xbm_path)
