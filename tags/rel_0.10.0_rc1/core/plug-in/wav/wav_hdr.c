/*
 * $Id: wav_hdr.c,v 1.6.2.2 2005/08/25 06:55:13 rco Exp $
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * sems is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "wav_hdr.h"
#include "../../log.h"

#include <string.h>
#include <sys/types.h>

#define SAFE_READ(buf,s,fp,sr) \
    sr = fread(buf,s,1,fp);\
    if((sr != 1) || ferror(fp)) return -1;

/* The file header of RIFF-WAVE files (*.wav).  Files are always in
   little-endian byte-order.  */

struct wav_header
{
  char      magic[4];
  u_int32_t length;
  char	    chunk_type[4];
  char      chunk_format[4];
  u_int32_t chunk_length;
  u_int16_t format;
  u_int16_t channels;
  u_int32_t sample_rate;
  u_int32_t bytes_per_second;
  u_int16_t sample_size;
  u_int16_t precision;
  char      chunk_data[4];
  u_int32_t data_length;
};


static int wav_read_header(FILE* fp, struct amci_file_desc_t* fmt_desc)
{
    unsigned int s;

    char tag[4]={'\0'};
    unsigned int file_size=0;
    unsigned int chunk_size=0;
    unsigned short fmt=0;
    unsigned short channels=0;
    unsigned int rate=0;
    unsigned short bits_per_sample=0;

    if(!fp)
	return -1;
    rewind(fp);

    DBG("trying to read WAV file\n");

    SAFE_READ(tag,4,fp,s);
    DBG("tag = <%.4s>\n",tag);
    if(strncmp(tag,"RIFF",4)){
	DBG("wrong format !");
	return -1;
    }

    SAFE_READ(&file_size,4,fp,s);
    DBG("file size = <%u>\n",file_size);

    SAFE_READ(tag,4,fp,s);
    DBG("tag = <%.4s>\n",tag);
    if(strncmp(tag,"WAVE",4)){
	DBG("wrong format !");
	return -1;
    }

    SAFE_READ(tag,4,fp,s);
    DBG("tag = <%.4s>\n",tag);
    if(strncmp(tag,"fmt ",4)){
	DBG("wrong format !");
	return -1;
    }
    
    SAFE_READ(&chunk_size,4,fp,s);
    DBG("chunk_size = <%u>\n",chunk_size);
    
    SAFE_READ(&fmt,2,fp,s);
    DBG("fmt = <%.2x>\n",fmt);

    SAFE_READ(&channels,2,fp,s);
    DBG("channels = <%i>\n",channels);

    SAFE_READ(&rate,4,fp,s);
    DBG("rate = <%i>\n",rate);

    /* do not read bytes/sec and block align */
    fseek(fp,6,SEEK_CUR);

    SAFE_READ(&bits_per_sample,2,fp,s);
    DBG("bits/sample = <%i>\n",bits_per_sample);

    fmt_desc->subtype = fmt;
    fmt_desc->sample = bits_per_sample>>3;
    fmt_desc->rate = rate;
    fmt_desc->channels = channels;

    if( (fmt == 0x01) && (fmt_desc->sample == 1)){
	ERROR("Sorry, we don't support PCM 8 bit\n");
	return -1;
    }

    fseek(fp,chunk_size-16,SEEK_CUR);

    for(;;) {

	SAFE_READ(tag,4,fp,s);
	DBG("tag = <%.4s>\n",tag);
	
	SAFE_READ(&chunk_size,4,fp,s);
	DBG("chunk size = <%i>\n",chunk_size);
	
	if(!strncmp(tag,"data",4))
	    break;

	fseek(fp,chunk_size,SEEK_CUR);
    }

    return 0;
}

int wav_open(FILE* fp, struct amci_file_desc_t* fmt_desc, int options, long h_codec)
{
    if(options == AMCI_RDONLY){
	return wav_read_header(fp,fmt_desc);
    }
    else {
	/*  Reserve some space for the header */
	/*  We need this, as information for headers  */
	/*  like 'size' is not known yet */
	fseek(fp,44L,SEEK_CUR); 
	return (ferror(fp) ? -1 : 0);
    }
}

int wav_write_header(FILE* fp, struct amci_file_desc_t* fmt_desc)
{
    struct wav_header hdr;

    memcpy(hdr.magic, "RIFF",4);
    hdr.length = fmt_desc->data_size + 36;
    memcpy(hdr.chunk_type, "WAVE",4);
    memcpy(hdr.chunk_format, "fmt ",4);
    hdr.chunk_length = 16;
    hdr.format = fmt_desc->subtype;
    hdr.channels = (unsigned short)fmt_desc->channels;
    hdr.sample_rate = (unsigned int)fmt_desc->rate;
    hdr.sample_size = hdr.channels * fmt_desc->sample;
    hdr.bytes_per_second = hdr.sample_rate * (unsigned int)hdr.sample_size;
    hdr.precision = (unsigned short)(fmt_desc->sample * 8);
    memcpy(hdr.chunk_data,"data",4);
    hdr.data_length=fmt_desc->data_size;

    fwrite(&hdr,sizeof(hdr),1,fp);
    if(ferror(fp)) return -1;

    DBG("fmt = <%i>\n",hdr.format);
    DBG("channels = <%i>\n",hdr.channels);
    DBG("rate = <%i>\n",hdr.sample_rate);
    DBG("data_size = <%i>\n",hdr.data_length);

    return 0;
}


int wav_close(FILE* fp, struct amci_file_desc_t* fmt_desc, int options, long h_codec)
{
    if(options == AMCI_WRONLY){
	rewind(fp);
	return wav_write_header(fp,fmt_desc);
    }
    return 0;
}









