% The Case of the Unusable Reusable
% django, python
% 2009-07-22

The [Django][1] web framework has a huge following that releases lots of
simple reusable apps that can be plugged into your website. However, sometimes
they are too simple, and not "plugin-able" quite enough.

Take for instance [django-favorites][2]. By itself it's a great package that
does exactly what you want--it allows a user to mark things as being a
favorite. Notice that I said "things." "Things" here means any model, because
it uses the [generic relationships][3] framework that Django provides.

This is a huge win on its own because it means that I don't have to define a
new favorites model for blog posts, one for photos and another for music I'm
listening too. But, there's a downside to this as well.

How do I check to see if an item is a favorite? Well, for each blog post I
select back from the database, I have to make another query to see whether or
not it was a favorite or not. Alternatively, of course, I could be smarter and
do a bulk query using an `IN` clause, making the query count only 2. But, if I
wasn't using an [ORM][4], my SQL would use an `OUTER JOIN`, or a sub-query to
select back whether or not it was marked a favorite for the current user.

Django can do this using the [`extra`][5] method for `QuerySet`s.

Ok, so we can use `extra` every time we want to select back whether or not an
item is a favorite, no big deal.

`content_type = ContentType.objects.get_for_model(Photo) SQL = """SELECT 1
FROM favorites_favorite f WHERE f.content_type_id = %(content_type)d and
f.object_id = photos_photo.id and f.user_id = %(user_id)d """ attrs =
{'content_type': content_type.id, 'user_id': user.id} entries =
Photo.objects.extra(select={'is_favorite': SQL % attrs}) `

Except that you have to do this every time. So, you then create a custom
manager for `Photo` that includes a `check_is_favorite` method, which adds the
`is_favorite` pseudo-column and everything is good.

That is until you have to do it for `Entry`, and `Song`, and `User`.

The solution however is simple. Reusable apps should include a "ManagerMixin",
if it makes sense to make reusing the app as painless as possible. The django-
favorites application that I've been using in my example would be complete in
my eyes if it had something like this defined in it:

`class FavoritesManagerMixin(object): """ A Mixin to add a
`favorite__favorite` column via extra """ def with_favorite_for(self, user,
all=True): """ Adds a column favorite__favorite to the returned object, which
indicates whether or not this item is a favorite for a user """ content_type =
ContentType.objects.get_for_model(self.model) pk_field = "%s.%s" %
(qn(self.model._meta.db_table), qn(self.model._meta.pk.column)) favorite_sql =
"""(SELECT 1 FROM %(favorites_db_table)s WHERE
%(favorites_db_table)s.object_id = %(pk_field)s and
%(favorites_db_table)s.content_type_id = %(content_type)d and
%(favorites_db_table)s.user_id = %(user_id)d) """ % {'pk_field': pk_field, \
'db_table': qn(self.model._meta.db_table), \ 'favorites_db_table':
qn(Favorite._meta.db_table), \ 'user_id': user.pk, \ 'content_type':
content_type.id, \ } extras = { 'select': {'favorite__favorite':
favorite_sql}, } if not all: extras['where'] = ['favorite__favorite == 1']
return self.extra(**extras) `

I have yet to run a bench mark against this to determine whether or not the
sub-query here is less efficient than doing an `OUTER JOIN`. My gut says it
would be, but for a first go at it, I'll keep it like this.

Anyway, then to make use of this you create a custom manager that uses
`FavoritesManagerMixin` as one of its base classes:

`class SongManager(models.Manager, **FavoritesMixinManager**): pass class
Song(models.Model): title = models.CharField(max_length=255, null=False,
blank=False) artist = models.ForeignKey('Artist') album =
models.ForeignKey('Album') **objects = SongManager()** `

And then to make use of it, we do:

`all_songs_with_favorites_marked = Song.objects.with_favorite_for(user)
only_favorite_songs = Song.objects.with_favorite_for(user, all=False) `

The intention of this post isn't to point out a flaw in django-favorites, an
otherwise great reusable application, BTW, but instead is meant to give a way
that this idea of reusable can actually be usable.

I put my django-favorites on [github][6]. Feel free to flame me for going
against what this post stands for and creating yet another "favorites"
reusable app, without submitting a patch to the original, I wanted some
practice in Django development.

   [1]: http://www.djangoproject.com/

   [2]: http://code.google.com/p/django-favorites/ (django-favorites on Google
Code)

   [3]: http://docs.djangoproject.com/en/dev/ref/contrib/contenttypes/#id1
(Generic Relations)

   [4]: http://en.wikipedia.org/wiki/Object-relational_mapping (Object
Relational Mapper)

   [5]: http://docs.djangoproject.com/en/dev/ref/models/querysets/#queryset-
methods-that-return-new-querysets

   [6]: http://github.com/apgwoz/django-favorites/

