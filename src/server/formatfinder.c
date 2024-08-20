char *formatfinder (unsigned char filetype[2]) {
    char *fileformat;
    switch (filetype[0]) {
        case 0xff:
            switch (filetype[1]) {
                case 0xd8:
                    fileformat = ".jpg";
                    break;
                case 0xfb:
                    fileformat = ".mp3";
                    break;
                default:
                    break;
            }
            break;
        case 0x25:
            switch (filetype[1]) {
                case 0x50:
                    fileformat = ".pdf";
                    break;
                default:
                    break;
            }
            break;
        case 0x50: // For .png files
            switch (filetype[1]) {
                case 0x4e:
                    fileformat = ".png";
                    break;
                default:
                    break;
            }
            break;
        case 0x4d: // For .mov files
            switch (filetype[1]) {
                case 0x4f:
                    fileformat = ".mov";
                    break;
                default:
                    break;
            }
            break;
        case 0x55: // For .zip files
            switch (filetype[1]) {
                case 0x0a:
                    fileformat = ".zip";
                    break;
                default:
                    break;
            }
            break;
        case 0x37: // For .7z files
            switch (filetype[1]) {
                case 0x7a:
                    fileformat = ".7z";
                    break;
                default:
                    break;
            }
            break;
        case 0x66: // For .mp4 files
            switch (filetype[1]) {
                case 0x74:
                    fileformat = ".mp4";
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    return fileformat;
}