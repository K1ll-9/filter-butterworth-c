# filter-butterworh-c
Simple Chebyshev and Butterworth implementation in C, double precision. Works well on macOS 13.4.1.

Features

 * lowpass
 * highpass
 * butterworth
 * chebyshev
 * ripple percent
   

This imlementation is based on algorithm from [https://exstrom.com/journal/sigproc/dsigproc.html](https://www.analog.com/media/en/technical-documentation/dsp-book/dsp_book_Ch20.pdf)

# Run example
make example
> ./example

# Steps to use a filter
Create a filter object using create_***_pass_filter(params...)

``` ChebFilter* filter = create_bw_low_pass_filter(ORDER, FC); ```

If fc is the cut-off frequency in hertz, the filter cut-off frequency must be expressed by fc/framerate. FC should be included in the [0, 0.5] range, 0.5 matching the Nyquist frequency.

```FC=(double)fc/FR```

For example, with a framerate at 44100Hz and a fc= 200Hz:

```FC=(double)200/44100```



Run the filter:

```double filtered_signal = applyfilter(filter, input[i]);```

Use filter to filter incoming doubles one by one. The filtered output is a double as well.

After using the filter, release the filter.

```free(filter);```
