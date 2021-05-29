
Sloppy (but faster) video slicing from second 100 or 20 seconds:

```
ffmpeg -ss 100 -i video.mp4 -t 20 out.mp4
```

Extract frames from a video:

```
ffmpeg -i v.mp4 frames-%03d.jpg
```

Extract only I-frames from a video

```
ffmpeg -i video.mp4 -vf "select=eq(pict_type\,I)" -vsync vfr -qscale:v 2 frames-%03d.jpeg
```

