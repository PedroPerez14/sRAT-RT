# Especifico una carpeta con dos subcarpetas (?) Mitsuba y sRAT-RT?
# Saca el RMSE y el CIE delta E 2000
# Prepara unos cuantos plots

# Plotea

# Hmmmm

# Hacer para la escena de osea vs sRAT-RT (mits vs normal, RGB y ShittyUplifting) a 5 10 20 y 50 metros (Jerlov I)

# Luego hacer para la escena de las pelotas (La tengo en mitsbua no??)

# Comparar la escena de la mochila guitarra solo en mi renderizador (?)
#           Una de estas tiene que llevar el LED azul (bolas???)

# Sobrecarga de triangulos / plot de tiempos para diferentes wavelengths???

# Hacer que pueda cambiar de escena? Y de agua de Jerlov??? hmmm


# Script begins here


import os
import cv2
import colour
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# TODO: Poner las facking rutas a los archivos
path_renders_mitsuba = 'C:/Users/Pedro/Desktop/Universidad/TFM/mitsuba-renders/reef_renders_osea/'
path_renders_rgb = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_rgb/'
path_renders_spectral = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_spec/'
path_renders_shittyuplifting ='C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_shittyuplift/'


def main():
    # Load images (mitsuba images are exr, the rest are png)
    # Compute RMSE
    # Compute CIE deltaE 2000
    # Visualize CIE deltaE 2000 ???
    # Plot everything very nicely

    images_mi = []
    images_rgb = []
    images_spec = []
    images_shitty = []

    for file in os.listdir(path_renders_mitsuba):
        if(file[-4:] == ".png"):
            images_mi += [path_renders_mitsuba+file]
    images_mi = sorted(images_mi)
    print(images_mi, '\n')

    for file in os.listdir(path_renders_rgb):
        images_rgb += [path_renders_rgb+file]
    images_rgb = sorted(images_rgb)
    print(images_rgb, '\n')

    for file in os.listdir(path_renders_spectral):
        images_spec += [path_renders_spectral+file]
    images_spec = sorted(images_spec)
    print(images_spec, '\n')

    for file in os.listdir(path_renders_shittyuplifting):
        images_shitty += [path_renders_shittyuplifting+file]
    images_shitty = sorted(images_shitty)
    print(images_shitty, '\n')

    
    im_mits = [None] * len(images_mi)
    im_srat_rgb = [None] * len(images_mi)
    im_srat_spec = [None] * len(images_mi)
    im_srat_shitty = [None] * len(images_mi)

    im_mits_lab = [None] * len(images_mi)
    im_rgb_lab = [None] * len(images_mi)
    im_spec_lab = [None] * len(images_mi)
    im_shitty_lab = [None] * len(images_mi)

    im_mits_display = [None] * len(images_mi)
    im_rgb_display = [None] * len(images_mi)
    im_spec_display = [None] * len(images_mi)
    im_shitty_display = [None] * len(images_mi)

    delta_E_rgb = [None] * len(images_mi)
    delta_E_spec = [None] * len(images_mi)
    delta_E_shitty = [None] * len(images_mi)

    RMSE_rgb = [None] * len(images_mi)
    RMSE_spec = [None] * len(images_mi)
    RMES_shitty = [None] * len(images_mi)

    # We are going to assume that the images are in depth order 
    # (5,10,20,50m), name them accordingly so the alphenumerical sorting orders them like that

    do_rmse = False
    for i in range(len(images_mi)):
        im_mits[i] = cv2.imread(images_mi[i])
        im_srat_rgb[i] = cv2.imread(images_rgb[i])
        im_srat_spec[i] = cv2.imread(images_spec[i])
        im_srat_shitty[i] = cv2.imread(images_shitty[i])

        im_mits_lab[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_RGB2Lab)
        im_rgb_lab[i] = cv2.cvtColor(im_srat_rgb[i], cv2.COLOR_RGB2Lab)
        im_spec_lab[i] = cv2.cvtColor(im_srat_spec[i], cv2.COLOR_RGB2Lab)
        im_shitty_lab[i] = cv2.cvtColor(im_srat_shitty[i], cv2.COLOR_RGB2Lab)

        im_mits_display[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_BGR2RGB )
        im_rgb_display[i] = cv2.cvtColor(im_srat_rgb[i], cv2.COLOR_BGR2RGB )
        im_spec_display[i] = cv2.cvtColor(im_srat_spec[i], cv2.COLOR_BGR2RGB )
        im_shitty_display[i] = cv2.cvtColor(im_srat_shitty[i], cv2.COLOR_BGR2RGB )

        if(not do_rmse):
            delta_E_rgb[i] = colour.delta_E(im_mits_lab[i], im_rgb_lab[i], 'CIE 2000')
            delta_E_spec[i] = colour.delta_E(im_mits_lab[i], im_spec_lab[i], 'CIE 2000')
            delta_E_shitty[i] = colour.delta_E(im_mits_lab[i], im_shitty_lab[i], 'CIE 2000')
        #else:
            # Compute the RMSE between the images TODO: colour.utilities.metric_mse !!!!s

    # Now do stuff outside the calculations loop
    # TODO: Esto está hardcodeado rico rico a mano, si quieres lo cambias Pedro del futuro
    ncols = len(images_mi) + 1   # 4, but we'll use 5 heheheheheh
    nrows = 7
    our_cmap = 'Spectral'
    rowtitles = []
    coltitles = ['depth=5m', 'depth=10m', 'depth=20m', 'depth=50m']
    if(do_rmse):
        rowtitles = ['Spectral Path Tracer (GT)', 'RGB (Ours)', 'Diff', 'Spectral Uplifting (Ours)', 'Diff', 'Poor Uplifting (Ours)', 'Diff']
    else:
        rowtitles = ['Spectral Path Tracer (GT)', 'RGB (Ours)', 'CIE delta E 2000', 'Spectral Uplifting (Ours)', 'CIE delta E 2000', 'Poor Uplifting (Ours)', 'CIE delta E 2000']

    fig = plt.figure(1) #figsize??
    gs = gridspec.GridSpec(nrows, ncols)
    gs.update(wspace = 0.001, hspace = 0.001)

    for i in range(nrows):
        for j in range(ncols):
            if i == 0 and j != (ncols-1):
                sp = fig.add_subplot(gs[i,j])
                sp.xaxis.set_label_position('top')
                sp.set_xlabel(coltitles[j], fontsize=7)
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize = 7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_mits_display[j])
                #sp.set_axis_off()
            elif i == 1 and j != (ncols - 1):               # and j != 0
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize = 7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_rgb_display[j])
                #sp.set_axis_off()
            elif i == 2:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 30.0], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize = 7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(delta_E_rgb[j], cmap=our_cmap)
                    #sp.set_axis_off()
            elif i == 3 and j != (ncols - 1):
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize = 7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_spec_display[j])
                #sp.set_axis_off()
            elif i == 4:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 30.0], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:  
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize = 7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(delta_E_spec[j], cmap=our_cmap)
                    #sp.set_axis_off()
            elif i == 5 and j != (ncols - 1):
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize = 7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_shitty_display[j])
                #sp.set_axis_off()
            elif i == 6:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 30.0], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize = 7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(delta_E_shitty[j], cmap=our_cmap)
                    #sp.set_axis_off()
    fig.tight_layout()
    fig.suptitle('Reef scene, Jerlov I water, XYZ response Curves')

    plt.show()

    # mean = np.mean(delta_E_rgb)
    # stdev = np.std(delta_E_rgb)
    # print("Mean delta E 2000: ", mean)
    # print("Std deviation delta E 2000: ", stdev)


    # plt.imshow(delta_E_rgb, cmap='Spectral')
    # plt.colorbar()
    # plt.show()

if __name__ == '__main__':
    main()