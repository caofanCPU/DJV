//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvFFmpegUtil.cpp

#include <djvFFmpegUtil.h>

#include <djvStringUtil.h>

//------------------------------------------------------------------------------
// djvFFmpegUtil::Dictionary
//------------------------------------------------------------------------------

djvFFmpegUtil::Dictionary::Dictionary() :
    _p(0)
{}

djvFFmpegUtil::Dictionary::~Dictionary()
{
    av_dict_free(&_p);
}

QMap<QString, QString> djvFFmpegUtil::Dictionary::map() const
{
    QMap<QString, QString> out;
    
    AVDictionaryEntry * entry = 0;
    
    while ((entry = av_dict_get(_p, "", entry, AV_DICT_IGNORE_SUFFIX)))
    {
        out.insert(entry->key, entry->value);
    }
    
    return out;
}

AVDictionary ** djvFFmpegUtil::Dictionary::operator () ()
{
    return &_p;
}

const AVDictionary * const * djvFFmpegUtil::Dictionary::operator () () const
{
    return &_p;
}

//------------------------------------------------------------------------------
// djvFFmpegUtil::Packet
//------------------------------------------------------------------------------

djvFFmpegUtil::Packet::Packet()
{
    av_init_packet(&_p);
}

djvFFmpegUtil::Packet::~Packet()
{
    av_free_packet(&_p);
}

AVPacket & djvFFmpegUtil::Packet::operator () ()
{
    return _p;
}

const AVPacket & djvFFmpegUtil::Packet::operator () () const
{
    return _p;
}

//------------------------------------------------------------------------------
// djvFFmpegUtil
//------------------------------------------------------------------------------

AVRational djvFFmpegUtil::timeBaseQ()
{
	AVRational r;
	r.num = 1;
	r.den = AV_TIME_BASE;
	return r;
}

QString djvFFmpegUtil::toString(int r)
{
    char buf [djvStringUtil::cStringLength];
    av_strerror(r, buf, djvStringUtil::cStringLength);
    return QString(buf);
}

