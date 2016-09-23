package com.mapswithme.maps.widget.placepage;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.text.TextUtils;

import com.mapswithme.maps.bookmarks.data.MapObject;
import com.mapswithme.maps.bookmarks.data.Metadata;
import com.mapswithme.maps.gallery.Image;
import com.mapswithme.maps.review.Review;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

@UiThread
public final class SponsoredHotel
{
  private static class Price
  {
    @NonNull
    final String mPrice;
    @NonNull
    final String mCurrency;

    private Price(@NonNull String price, @NonNull String currency)
    {
      mPrice = price;
      mCurrency = currency;
    }
  }

  static class FacilityType
  {
    @NonNull
    private final String mKey;
    @NonNull
    private final String mName;

    public FacilityType(@NonNull String key, @NonNull String name)
    {
      mKey = key;
      mName = name;
    }

    @NonNull
    public String getKey()
    {
      return mKey;
    }

    @NonNull
    public String getName()
    {
      return mName;
    }
  }

  static class NearbyObject
  {
    @NonNull
    private final String mCategory;
    @NonNull
    private final String mTitle;
    @NonNull
    private final String mDistance;
    private final double mLatitude;
    private final double mLongitude;

    public NearbyObject(@NonNull String category, @NonNull String title,
                        @NonNull String distance, double lat, double lon)
    {
      mCategory = category;
      mTitle = title;
      mDistance = distance;
      mLatitude = lat;
      mLongitude = lon;
    }

    @NonNull
    public String getCategory()
    {
      return mCategory;
    }

    @NonNull
    public String getTitle()
    {
      return mTitle;
    }

    @NonNull
    public String getDistance()
    {
      return mDistance;
    }

    public double getLatitude()
    {
      return mLatitude;
    }

    public double getLongitude()
    {
      return mLongitude;
    }
  }

  static class HotelInfo
  {
    @Nullable
    final String mDescription;
    @Nullable
    final Image[] mPhotos;
    @Nullable
    final FacilityType[] mFacilities;
    @Nullable
    final Review[] mReviews;
    @Nullable
    final NearbyObject[] mNearby;

    public HotelInfo(@Nullable String description, @Nullable Image[] photos,
                     @Nullable FacilityType[] facilities, @Nullable Review[] reviews,
                     @Nullable NearbyObject[] nearby)
    {
      mDescription = description;
      mPhotos = photos;
      mFacilities = facilities;
      mReviews = reviews;
      mNearby = nearby;
    }
  }

  interface OnPriceReceivedListener
  {
    /**
     * This method is called from the native core on the UI thread
     * when the Hotel price will be obtained
     *
     * @param id A hotel id
     * @param price A price
     * @param currency A price currency
     */
    @UiThread
    void onPriceReceived(@NonNull String id, @NonNull String price, @NonNull String currency);
  }

  interface OnInfoReceivedListener
  {
    /**
     * This method is called from the native core on the UI thread
     * when the Hotel information will be obtained
     *
     * @param id A hotel id
     * @param info A hotel info
     */
    @UiThread
    void onInfoReceived(@NonNull String id, @NonNull HotelInfo info);
  }

  // Hotel ID -> Price
  @NonNull
  private static final Map<String, Price> sPriceCache = new HashMap<>();
  // Hotel ID -> Description
  @NonNull
  private static final Map<String, HotelInfo> sInfoCache = new HashMap<>();
  @NonNull
  private static WeakReference<OnPriceReceivedListener> sPriceListener = new WeakReference<>(null);
  @NonNull
  private static WeakReference<OnInfoReceivedListener> sInfoListener = new WeakReference<>(null);

  @Nullable
  private String mId;

  @NonNull
  final String mRating;
  @NonNull
  final String mPrice;
  @NonNull
  final String mUrlBook;
  @NonNull
  final String mUrlDescription;

  public SponsoredHotel(@NonNull String rating, @NonNull String price, @NonNull String urlBook,
                        @NonNull String urlDescription)
  {
    mRating = rating;
    mPrice = price;
    mUrlBook = urlBook;
    mUrlDescription = urlDescription;
  }

  void updateId(MapObject point)
  {
    mId = point.getMetadata(Metadata.MetadataType.FMD_SPONSORED_ID);
  }

  @Nullable
  public String getId()
  {
    return mId;
  }

  @NonNull
  public String getRating()
  {
    return mRating;
  }

  @NonNull
  public String getPrice()
  {
    return mPrice;
  }

  @NonNull
  public String getUrlBook()
  {
    return mUrlBook;
  }

  @NonNull
  public String getUrlDescription()
  {
    return mUrlDescription;
  }

  static void setPriceListener(@NonNull OnPriceReceivedListener listener)
  {
    sPriceListener = new WeakReference<>(listener);
  }

  static void setInfoListener(@NonNull OnInfoReceivedListener listener)
  {
    sInfoListener = new WeakReference<>(listener);
  }

  /**
   * Make request to obtain hotel price information.
   * This method also checks cache for requested hotel id
   * and if cache exists - call {@link #onPriceReceived(String, String, String) onPriceReceived} immediately
   *
   * @param id A Hotel id
   * @param currencyCode A user currency
   */
  static void requestPrice(String id, String currencyCode)
  {
    Price p = sPriceCache.get(id);
    if (p != null)
      onPriceReceived(id, p.mPrice, p.mCurrency);

    nativeRequestPrice(id, currencyCode);
  }

  /**
   * Make request to obtain hotel information.
   * This method also checks cache for requested hotel id
   * and if cache exists - call {@link #onInfoReceived(String, HotelInfo) onInfoReceived} immediately
   *
   * @param id A Hotel id
   * @param locale A user locale
   */
  static void requestInfo(String id, String locale)
  {
    HotelInfo info = sInfoCache.get(id);
    if (info != null)
      onInfoReceived(id, info);

    nativeRequestInfo(id, locale);
  }

  private static void onPriceReceived(@NonNull String id, @NonNull String price,
                                      @NonNull String currency)
  {
    if (TextUtils.isEmpty(price))
      return;

    sPriceCache.put(id, new Price(price, currency));


    OnPriceReceivedListener listener = sPriceListener.get();
    if (listener != null)
      listener.onPriceReceived(id, price, currency);
  }

  private static void onInfoReceived(@NonNull String id, @NonNull HotelInfo info)
  {
    sInfoCache.put(id, info);

    OnInfoReceivedListener listener = sInfoListener.get();
    if (listener != null)
      listener.onInfoReceived(id, info);
  }

  @Nullable
  public static native SponsoredHotel nativeGetCurrent();

  private static native void nativeRequestPrice(@NonNull String id, @NonNull String currencyCode);

  private static native void nativeRequestInfo(@NonNull String id, @NonNull String locale);
}
