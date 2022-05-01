/*
 Copyright 2021-2022 Yury Bobylev <bobilev_yury@mail.ru>

 This file is part of Money.
 Money is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.
 Money is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Money. If not,
 see <https://www.gnu.org/licenses/>.
 */

#include <gtkmm.h>
#include <libintl.h>
#include "MoneyApplication.h"

int
main (int argc, char *argv[])
{
  AuxFunc af;
  std::string Sharepath;
  std::filesystem::path p (std::filesystem::u8path (af.get_selfpath ()));
  Sharepath = p.parent_path ().u8string () + "/../share/Money";
  Sharepath = Sharepath + "/Translations.d";
  bindtextdomain ("Money", Sharepath.c_str ());
  bind_textdomain_codeset ("Money", "UTF-8");
  textdomain ("Money");
  auto app = MoneyApplication::create ();
  return app->run (argc, argv);
}
