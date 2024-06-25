# Especifico una carpeta con dos subcarpetas (?) Mitsuba y sRAT-RT?
# Saca el RMSE y el CIE delta E 2000
# Prepara unos cuantos plots

# Plotea

# Hmmmm

# Hacer para la escena de osea vs sRAT-RT (mits vs normal, RGB y ShittyUplifting) a 5 10 20 y 50 metros (Jerlov I)

# Luego hacer para la escena de las pelotas (La tengo en mitsbua no??)

# Comparar la escena de la mochila guitarra solo en mi renderizador (?)
#           Una de estas tiene que llevar el LED azul (bolas???)

# Hacer que pueda cambiar de escena? Y de agua de Jerlov??? hmmm

# Script begins here

import OpenEXR as exr
import os
os.environ["OPENCV_IO_ENABLE_OPENEXR"]="1"
import cv2

import numpy as np
import math
import colour
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec




def hdr_to_ldr(img, exposure=1.0, out_dtype=np.uint8):
    img *= exposure # exposure if needed
    img = img**(1/2.2) # gamma correction
    # clip
    img = np.clip(img, 0, 1)
    return (img * 255).astype(out_dtype)

def reef_scene_plots():
    path_renders_mitsuba = 'C:/Users/Pedro/Desktop/Universidad/TFM/mitsuba-renders/reef_renders_osea/'
    path_renders_rgb = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_rgb/'
    path_renders_spectral = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_spec/'
    path_renders_shittyuplifting ='C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_shittyuplift/'
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
        if(file[-4:] == ".exr"):
            images_mi += [path_renders_mitsuba+file]
    images_mi = sorted(images_mi)
    print(images_mi, '\n')

    for file in os.listdir(path_renders_rgb):
        if(file[-4:] == ".png"):
            images_rgb += [path_renders_rgb+file]
    images_rgb = sorted(images_rgb)
    print(images_rgb, '\n')

    for file in os.listdir(path_renders_spectral):
        if(file[-4:] == ".png"):
            images_spec += [path_renders_spectral+file]
    images_spec = sorted(images_spec)
    print(images_spec, '\n')

    for file in os.listdir(path_renders_shittyuplifting):
        if(file[-4:] == ".png"):
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

    ############################################ For metrics

    delta_E_rgb = [None] * len(images_mi)
    delta_E_spec = [None] * len(images_mi)
    delta_E_shitty = [None] * len(images_mi)

    RMSE_rgb = [None] * len(images_mi)
    RMSE_spec = [None] * len(images_mi)
    RMSE_shitty = [None] * len(images_mi)

    diff_rgb = [None] * len(images_mi)
    diff_spec = [None] * len(images_mi)
    diff_shitty = [None] * len(images_mi)

    # We are going to assume that the images are in depth order 
    # (1,5,10,20,50m) name them accordingly so the alphenumerical sorting orders them like that

    do_rmse = False
    for i in range(len(images_mi)):
        im_mits[i] = cv2.imread(images_mi[i], cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
        im_mits[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_BGR2RGB )
        im_mits[i] = hdr_to_ldr(im_mits[i], exposure=1.0)
        im_srat_rgb[i] = cv2.cvtColor(cv2.imread(images_rgb[i]), cv2.COLOR_BGR2RGB )
        im_srat_spec[i] = cv2.cvtColor(cv2.imread(images_spec[i]), cv2.COLOR_BGR2RGB )
        im_srat_shitty[i] = cv2.cvtColor(cv2.imread(images_shitty[i]), cv2.COLOR_BGR2RGB )

        im_mits_lab[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_RGB2Lab)
        im_rgb_lab[i] = cv2.cvtColor(im_srat_rgb[i], cv2.COLOR_RGB2Lab)
        im_spec_lab[i] = cv2.cvtColor(im_srat_spec[i], cv2.COLOR_RGB2Lab)
        im_shitty_lab[i] = cv2.cvtColor(im_srat_shitty[i], cv2.COLOR_RGB2Lab)

        delta_E_rgb[i] = colour.delta_E(im_mits_lab[i], im_rgb_lab[i], 'CIE 2000')
        delta_E_spec[i] = colour.delta_E(im_mits_lab[i], im_spec_lab[i], 'CIE 2000')
        delta_E_shitty[i] = colour.delta_E(im_mits_lab[i], im_shitty_lab[i], 'CIE 2000')
        
        RMSE_rgb[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_rgb[i] ) )
        RMSE_spec[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_spec[i] ))
        RMSE_shitty[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_shitty[i] ))

        diff_rgb[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_rgb[i] ), axis=2)
        diff_spec[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_spec[i] ), axis=2)
        diff_shitty[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_shitty[i] ), axis=2)


    # Now do stuff outside the calculations loop
    # TODO: Esto está hardcodeado rico rico a mano, si quieres lo cambias Pedro del futuro
    ncols = len(images_mi) + 1   # 4, but we'll use 5 heheheheheh
    nrows = 7
    our_cmap = 'hot'
    rowtitles = []
    coltitles = ['depth=1m', 'depth=5m', 'depth=10m', 'depth=20m', 'depth=50m']
    if(do_rmse):
        rowtitles = ['Spectral Path Tracer (GT)', 'RGB (Ours)', 'Diff', 'Spectral Uplifting (Ours)', 'Diff', 'Poor Uplifting', 'Diff']
    else:
        rowtitles = ['Spectral Path Tracer (GT)', 'RGB (Ours)', 'Abs. difference', 'Spectral Uplifting (Ours)', 'Abs. difference', 'Poor Uplifting', 'Abs. difference']

    fig = plt.figure(1)
    gs = gridspec.GridSpec(nrows, ncols)
    gs.update(wspace = 0.001, hspace = 0.001)

    for i in range(nrows):
        for j in range(ncols):
            if i == 0 and j != (ncols-1):
                sp = fig.add_subplot(gs[i,j])
                sp.xaxis.set_label_position('top')
                sp.set_xlabel(coltitles[j], fontsize=14)
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize =7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_mits[j])
                #sp.set_axis_off()
            elif i == 1 and j != (ncols - 1):               # and j != 0
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize=7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_srat_rgb[j])
                #sp.set_axis_off()
            elif i == 2:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 50], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize=7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(diff_rgb[j], cmap=our_cmap, vmin=0.0, vmax=50)
                    txt = f"RMSE={RMSE_rgb[j]:.2f}\ndE2000={np.mean(delta_E_rgb[j]):.2f}"
                    sp.text(0.95, 0.85, txt, bbox={'facecolor': 'white', 'pad': 2, 'alpha': 0.5, 'edgecolor': 'none'}, ha="right", va="top", fontsize=9, transform=sp.transAxes)
                    #sp.set_axis_off()
            elif i == 3 and j != (ncols - 1):
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize=7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_srat_spec[j])
                #sp.set_axis_off()
            elif i == 4:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 50], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:  
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize=7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(diff_spec[j], cmap=our_cmap, vmin=0.0, vmax=50)
                    txt = f"RMSE={RMSE_spec[j]:.2f}\ndE2000={np.mean(delta_E_spec[j]):.0f}"
                    sp.text(0.95, 0.85, txt, bbox={'facecolor': 'white', 'pad': 2, 'alpha': 0.5, 'edgecolor': 'none'}, ha="right", va="top", fontsize=9, transform=sp.transAxes)
                    #sp.set_axis_off()
            elif i == 5 and j != (ncols - 1):
                sp = fig.add_subplot(gs[i,j])
                if j == 0:
                    sp.set_ylabel(rowtitles[i], fontsize=7)
                sp.set_yticks([])
                sp.set_xticks([])
                #else:
                sp.imshow(im_srat_shitty[j])
                #sp.set_axis_off()
            elif i == 6:
                sp = fig.add_subplot(gs[i,j])
                if j == (ncols-1):
                    plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, 50], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                    sp.set_axis_off()
                else:
                    if j == 0:
                        sp.set_ylabel(rowtitles[i], fontsize=7)
                    sp.set_yticks([])
                    sp.set_xticks([])
                    #else:
                    last_im = sp.imshow(diff_shitty[j], cmap=our_cmap, vmin=0.0, vmax=50)
                    txt = f"RMSE={RMSE_shitty[j]:.2f}\ndE2000={np.mean(delta_E_shitty[j]):.0f}"
                    sp.text(0.95, 0.85, txt, bbox={'facecolor': 'white', 'pad': 2, 'alpha': 0.5, 'edgecolor': 'none'}, ha="right", va="top", fontsize=9, transform=sp.transAxes)
                    #sp.set_axis_off()
    fig.tight_layout()
    fig.suptitle('Reef scene, Jerlov I water, XYZ response Curves')

    plt.show()
    # print(f"Image {names[i]} error: {np.mean(error):.2f} ({np.std(error):.2f}), removing zeros: {np.mean(error[error>10])} ({np.std(error[error>10]):.2f})")

    mean_dE_rgb = np.mean(delta_E_rgb)
    mean_dE_spec = np.mean(delta_E_spec)
    mean_dE_shitty = np.mean(delta_E_shitty)

    mean_rmse_rgb = np.mean(RMSE_rgb)
    mean_rmse_spec = np.mean(RMSE_spec)
    mean_rmse_shitty = np.mean(RMSE_shitty)
    # print(mean_dE_rgb)
    plot_histogram(mean_dE_rgb, mean_dE_spec, mean_dE_shitty, color='blue', legend=["CIE dE 2000"], ylabel="Perceptual error scores")
    plot_histogram(mean_rmse_rgb, mean_rmse_spec, mean_rmse_shitty, color='green', legend=["RMSE"], ylabel="Error scores")
    plot_hist(im_srat_rgb, im_mits , coltitles, "Reef Scene, RGB Images")
    plot_hist(im_srat_spec, im_mits , coltitles, "Reef Scene, Naive Upsampling Images")
    plot_hist(im_srat_shitty, im_mits , coltitles, "Reef Scene, Spectral upsampling Images")

    
def balls_scene_plots(path_renders_mitsuba, path_renders_rgb, path_renders_spectral, path_renders_shittyuplifting, plot_suptitle, our_vmax):
    # path_renders_mitsuba = 'C:/Users/Pedro/Desktop/Universidad/TFM/mitsuba-renders/spheres_new/'
    # path_renders_rgb = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_rgb/spheres_d65/'
    # path_renders_spectral = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_spec/spheres_d65/'
    # path_renders_shittyuplifting ='C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_shittyuplift/spheres_d65/'
    
    images_mi = []
    images_rgb = []
    images_spec = []
    images_shitty = []

    for file in os.listdir(path_renders_mitsuba):
        if(file[-4:] == ".exr"):
            images_mi += [path_renders_mitsuba+file]
    images_mi = sorted(images_mi)
    print("Images MI: ", images_mi, '\n')

    for file in os.listdir(path_renders_rgb):
        images_rgb += [path_renders_rgb+file]
    images_rgb = sorted(images_rgb)
    print("Images RGB: ", images_rgb, '\n')

    for file in os.listdir(path_renders_spectral):
        images_spec += [path_renders_spectral+file]
    images_spec = sorted(images_spec)
    print("Images SPEC: ", images_spec, '\n')

    for file in os.listdir(path_renders_shittyuplifting):
        images_shitty += [path_renders_shittyuplifting+file]
    images_shitty = sorted(images_shitty)
    print("Images SHITTY: ", images_shitty, '\n')

    im_mits = [None] * len(images_mi)
    im_srat_rgb = [None] * len(images_mi)
    im_srat_spec = [None] * len(images_mi)
    im_srat_shitty = [None] * len(images_mi)

    im_mits_lab = [None] * len(images_mi)
    im_rgb_lab = [None] * len(images_mi)
    im_spec_lab = [None] * len(images_mi)
    im_shitty_lab = [None] * len(images_mi)

    ############################################ For metrics

    delta_E_rgb = [None] * len(images_mi)
    delta_E_spec = [None] * len(images_mi)
    delta_E_shitty = [None] * len(images_mi)

    RMSE_rgb = [None] * len(images_mi)
    RMSE_spec = [None] * len(images_mi)
    RMSE_shitty = [None] * len(images_mi)

    diff_rgb = [None] * len(images_mi)
    diff_spec = [None] * len(images_mi)
    diff_shitty = [None] * len(images_mi)

    # We are going to assume that the images are in depth order 
    # (1,5,10,20, 50m), name them accordingly so the alphenumerical sorting orders them like that

    for i in range(len(images_mi)):
        im_mits[i] = cv2.imread(images_mi[i], cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
        im_mits[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_BGR2RGB )
        im_mits[i] = hdr_to_ldr(im_mits[i], exposure=1.0)
        im_srat_rgb[i] = cv2.cvtColor(cv2.imread(images_rgb[i]), cv2.COLOR_BGR2RGB )
        im_srat_spec[i] = cv2.cvtColor(cv2.imread(images_spec[i]), cv2.COLOR_BGR2RGB )
        im_srat_shitty[i] = cv2.cvtColor(cv2.imread(images_shitty[i]), cv2.COLOR_BGR2RGB )

        im_mits_lab[i] = cv2.cvtColor(im_mits[i], cv2.COLOR_RGB2Lab)
        im_rgb_lab[i] = cv2.cvtColor(im_srat_rgb[i], cv2.COLOR_RGB2Lab)
        im_spec_lab[i] = cv2.cvtColor(im_srat_spec[i], cv2.COLOR_RGB2Lab)
        im_shitty_lab[i] = cv2.cvtColor(im_srat_shitty[i], cv2.COLOR_RGB2Lab)

        delta_E_rgb[i] = colour.delta_E(im_mits_lab[i], im_rgb_lab[i], 'CIE 2000')
        delta_E_spec[i] = colour.delta_E(im_mits_lab[i], im_spec_lab[i], 'CIE 2000')
        delta_E_shitty[i] = colour.delta_E(im_mits_lab[i], im_shitty_lab[i], 'CIE 2000')
        
        RMSE_rgb[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_rgb[i] ) )
        RMSE_spec[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_spec[i] ))
        RMSE_shitty[i] = math.sqrt(colour.utilities.metric_mse(im_mits[i] , im_srat_shitty[i] ))

        diff_rgb[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_rgb[i] ), axis=2)
        diff_spec[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_spec[i] ), axis=2)
        diff_shitty[i] = np.mean(cv2.absdiff(im_mits[i] , im_srat_shitty[i] ), axis=2)


    # Now do stuff outside the calculations loop
    # TODO: Esto está hardcodeado rico rico a mano, si quieres lo cambias Pedro del futuro
    ncols = len(images_mi) + 2   # we only have 1 image this time, but we'll plot the error and the bar alongside it, hence the +2
    nrows = 3
    our_cmap = 'hot'

    coltitles = ['Our result', 'Image absolute difference']
    #rowtitles = ['Spectral Path Tracer (GT)', 'RGB (Ours)', 'Abs. difference', 'Spectral Uplifting (Ours)', 'Abs. difference', 'Poor Uplifting', 'Abs. difference']
    rowtitles = ['RGB (Ours)', 'Spectral Uplifting (Ours)', 'Naive Uplifting']

    fig = plt.figure(1)
    gs = gridspec.GridSpec(nrows, ncols)
    gs.update(wspace = 0.001, hspace = 0.001)


    img_to_plot = None
    diff_to_plot = None
    rmse_to_plot = None
    dE_to_plot = None
    for i in range(nrows):
        for j in range(ncols):
            if i == 0:      # RGB
                # img_to_plot --> rgb
                img_to_plot = im_srat_rgb[0]
                diff_to_plot = diff_rgb[0]
                rmse_to_plot = RMSE_rgb[0]
                dE_to_plot = delta_E_rgb[0]
            elif i == 1:    # Spectral
                # img_to_plot --> spec
                img_to_plot = im_srat_spec[0]
                diff_to_plot = diff_spec[0]
                rmse_to_plot = RMSE_spec[0]
                dE_to_plot = delta_E_spec[0]
            elif i == 2:    # Naive
                # img_to_plot --> naive
                img_to_plot = im_srat_shitty[0]
                diff_to_plot = diff_shitty[0]
                rmse_to_plot = RMSE_shitty[0]
                dE_to_plot = delta_E_shitty[0]                    

            sp = fig.add_subplot(gs[i,j])
            sp.set_yticks([])
            sp.set_xticks([])
            if j == 0:              # Plot render
                sp.set_ylabel(rowtitles[i], fontsize=14)
                sp.imshow(img_to_plot)
                if i == 2:
                    sp.set_xlabel(coltitles[j], fontsize=14)
            elif j == 1:            # Plot diff
                last_im = sp.imshow(diff_to_plot, cmap=our_cmap, vmin=0.0, vmax=our_vmax)
                txt = f"RMSE={rmse_to_plot:.2f}\ndE2000={np.mean(dE_to_plot):.0f}"
                sp.text(0.95, 0.2, txt, bbox={'facecolor': 'white', 'pad': 2, 'alpha': 0.5, 'edgecolor': 'none'}, ha="right", va="top", fontsize=14, transform=sp.transAxes)
                if i == 2:
                    sp.set_xlabel(coltitles[j], fontsize=14)
            else:                   # Plot colorbar
                plt.colorbar(last_im, ax=sp, use_gridspec=True, cmap=our_cmap, ticks=[0, our_vmax], orientation='vertical', aspect=20, fraction = 1.0, shrink=0.95)
                sp.set_axis_off() 

    fig.tight_layout()
    fig.suptitle(plot_suptitle)
    plt.show()
    plt.close()
     
    # Histogram stuff
    mean_dE_rgb = np.mean(delta_E_rgb)
    mean_dE_spec = np.mean(delta_E_spec)
    mean_dE_shitty = np.mean(delta_E_shitty)

    mean_rmse_rgb = np.mean(RMSE_rgb)
    mean_rmse_spec = np.mean(RMSE_spec)
    mean_rmse_shitty = np.mean(RMSE_shitty)
    plot_histogram(mean_dE_rgb, mean_dE_spec, mean_dE_shitty, color='blue', legend=["CIE dE 2000"], ylabel="Perceptual error scores")
    plot_histogram(mean_rmse_rgb, mean_rmse_spec, mean_rmse_shitty, color='green', legend=["RMSE"], ylabel="Error scores")
    plot_hist(im_srat_rgb, im_mits , coltitles, "Spheres D65 Scene, RGB Images")
    plot_hist(im_srat_spec, im_mits , coltitles, "Spheres D65 Scene, Naive Upsampling Images")
    plot_hist(im_srat_shitty, im_mits , coltitles, "Spheres D65 Scene, Spectral upsampling Images")

def addlabels(x,y):
    for i in range(len(x)):
        plt.text(i-0.2, y[i], "{:.4f}".format(y[i]))

# TODO: Nestor's code
#     plot_hist(im_srat_rgb, im_mits , coltitles, "Reef Scene, RGB Images", mean_rmse_rgb)
def plot_hist(images, ref_im_ldr, names, suptitle):
    fig, axs = plt.subplots(1, len(images), figsize=(len(images) * 5, 5))  
    for i, im in enumerate(images):
        error = np.abs(ref_im_ldr - im)
        error = np.mean(error, axis=-1)
        if len(images) == 1:
            axs.hist(error.ravel(), bins=100, range=(0, 255))
            axs.set_yscale("log")
            axs.set_title(names[i])
        else:
            axs[i].hist(error.ravel(), bins=100, range=(0, 255))
            axs[i].set_yscale("log")
            axs[i].set_title(names[i])
    fig.suptitle(suptitle)
    fig.tight_layout()
    plt.show()
    plt.close()

def plot_histogram(mean_rgb, mean_spec, mean_shitty, color='blue', legend=["CIE dE 2000"], ylabel="Perceptual error scores"):
    # create data 
    x = np.arange(3) 
    y1 = [mean_rgb, mean_spec, mean_shitty]
    #y2 = [mean_rmse_rgb, mean_rmse_spec, mean_rmse_shitty]
    width = 0.75
    # plot data in grouped manner of bar type 
    plt.bar(x, y1, width, color=color) 
    addlabels(x, y1)
    #plt.bar(x, y2, width, color='orange') 
    plt.xticks(x, ['RGB (Ours)', 'Spectral Uplifting (Ours)', 'Naive Uplifting']) 
    #plt.xlabel("Techniques used for real-time rendering") 
    plt.ylabel(ylabel=ylabel) 
    plt.legend(legend) 
    plt.show() 
    plt.close()

def main():
    ###################### REEF SCENE ######################
    reef_scene_plots()


    ##################### BALLS SCENE 1 #####################
    path_renders_mitsuba = 'C:/Users/Pedro/Desktop/Universidad/TFM/mitsuba-renders/spheres_new/'
    path_renders_rgb = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_rgb/spheres_d65/'
    path_renders_spectral = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_spec/spheres_d65/'
    path_renders_shittyuplifting ='C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_shittyuplift/spheres_d65/'
    balls_scene_plots(path_renders_mitsuba, path_renders_rgb, 
                      path_renders_spectral, path_renders_shittyuplifting, 
                      plot_suptitle='Spheres scene, D65 Illuminant, XYZ response Curves', our_vmax=20)
    
    ##################### BALLS SCENE 2 #####################
    path_renders_mitsuba = 'C:/Users/Pedro/Desktop/Universidad/TFM/mitsuba-renders/spheres_led/'
    path_renders_rgb = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_rgb/spheres_led/'
    path_renders_spectral = 'C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_spec/spheres_led/'
    path_renders_shittyuplifting ='C:/Users/Pedro/Desktop/Universidad/TFM/OpenGL/sRAT-RT/resources/screenshots/screenshots_shittyuplift/spheres_led/'
    balls_scene_plots(path_renders_mitsuba, path_renders_rgb, 
                      path_renders_spectral, path_renders_shittyuplifting, 
                      plot_suptitle='Spheres scene, blue LED Illuminant, XYZ response Curves', our_vmax=100)
    


if __name__ == '__main__':
    main()

    

    