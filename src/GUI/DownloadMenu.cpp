/*
 Copyright 2021-2023 Yury Bobylev <bobilev_yury@mail.ru>

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

#include "DownloadMenu.h"

DownloadMenu::DownloadMenu(Gtk::Window *mwin)
{
  Mwin = mwin;
}

DownloadMenu::~DownloadMenu()
{
  // TODO Auto-generated destructor stub
}

void
DownloadMenu::downloadMenu()
{
  AuxFunc af;
  std::vector<std::tuple<std::string, std::string, std::string>> instruments;
  std::vector<std::tuple<std::string, std::string, std::string>> instrumentsf;
  instruments = formVectorInstr(1);
  std::sort(instruments.begin(), instruments.end(), [&af]
  (auto &el1, auto &el2)
    {
      std::string l1 = std::get<2>(el1);
      std::string l2 = std::get<2>(el2);
      l1.erase(std::remove_if(l1.begin(), l1.end(), [](auto &el)
		{
		  return el == '\"';
		}), l1.end());
      l2.erase(std::remove_if(l2.begin(), l2.end(), [](auto &el)
		{
		  return el == '\"';
		}), l2.end());
      while(*(l1.begin()) == ' ')
	{
	  l1.erase(l1.begin());
	}
      while(*(l2.begin()) == ' ')
	{
	  l2.erase(l2.begin());
	}
      af.stringToLower(l1);
      af.stringToLower(l2);
      return l1 < l2;
    });
  instrumentsf = formVectorInstr(2);
  std::sort(instrumentsf.begin(), instrumentsf.end(), [&af]
  (auto &el1, auto &el2)
    {
      std::string l1 = std::get<2>(el1);
      std::string l2 = std::get<2>(el2);
      l1.erase(std::remove_if(l1.begin(), l1.end(), [](auto &el)
		{
		  return el == '\"';
		}), l1.end());
      l2.erase(std::remove_if(l2.begin(), l2.end(), [](auto &el)
		{
		  return el == '\"';
		}), l2.end());
      while(*(l1.begin()) == ' ')
	{
	  l1.erase(l1.begin());
	}
      while(*(l2.begin()) == ' ')
	{
	  l2.erase(l2.begin());
	}
      af.stringToLower(l1);
      af.stringToLower(l2);
      return l1 < l2;
    });
  std::copy(instrumentsf.begin(), instrumentsf.end(),
	    std::back_inserter(instruments));
  instrumentsf.clear();

  if(instruments.size() > 0)
    {
      Gtk::Window *window = new Gtk::Window;
      window->set_application(Mwin->get_application());
      window->set_default_size(1, 1);
      window->set_name("downloadWindow");
      window->set_title(gettext("Download menu"));
      window->set_transient_for(*Mwin);
      window->set_modal(true);

      Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
      grid->set_halign(Gtk::Align::FILL);
      grid->set_valign(Gtk::Align::FILL);
      window->set_child(*grid);

      Gtk::TreeModel::ColumnRecord record;
      Gtk::TreeModelColumn<Glib::ustring> instrument;
      Gtk::TreeModelColumn<Glib::ustring> code;
      Gtk::TreeModelColumn<Glib::ustring> instrfnm;
      record.add(instrument);
      record.add(instrfnm);
      record.add(code);
      Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(record);
      for(size_t i = 0; i < instruments.size(); i++)
	{
	  auto row = *(model->append());
	  row[instrument] = Glib::ustring(std::get<2>(instruments[i])) + "-"
	      + Glib::ustring(std::get<0>(instruments[i]));
	  row[instrfnm] = Glib::ustring(std::get<1>(instruments[i]));
	  row[code] = Glib::ustring(std::get<0>(instruments[i]));
	}

      Gtk::TreeView *box = Gtk::make_managed<Gtk::TreeView>();
      box->set_name("treeViewI");

      Gtk::ScrolledWindow *scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
      scroll->set_policy(Gtk::PolicyType::AUTOMATIC,
			 Gtk::PolicyType::AUTOMATIC);
      scroll->set_child(*box);

      Gtk::Popover *pop = Gtk::make_managed<Gtk::Popover>();
      pop->set_child(*scroll);

      Gtk::MenuButton *mb = Gtk::make_managed<Gtk::MenuButton>();
      mb->set_margin(5);
      mb->set_name("boards");
      mb->set_popover(*pop);
      auto row_instr = model->children()[0];
      if(row_instr)
	{
	  Glib::ustring val;
	  row_instr.get_value(0, val);
	  mb->set_label(val);
	}
      grid->attach(*mb, 0, 0, 1, 1);

      box->append_column("", instrument);
      box->set_headers_visible(false);
      box->set_activate_on_single_click(true);
      box->signal_row_activated().connect(
	  sigc::bind(sigc::mem_fun(*this, &DownloadMenu::instrSelection), model,
		     mb));

      Gtk::Button *instrdeals = Gtk::make_managed<Gtk::Button>();
      instrdeals->set_name("allButton");
      instrdeals->set_label(gettext("Transactions for instrument"));
      instrdeals->set_margin(5);
      Gtk::Window *mw = Mwin;
      instrdeals->signal_clicked().connect([window, box, mw]
      {
	DownloadMenu dm(mw);
	dm.downlodSinglInstrDeals(window, box);
      });
      grid->attach(*instrdeals, 0, 1, 1, 1);

      Gtk::Button *instrall = Gtk::make_managed<Gtk::Button>();
      instrall->set_name("allButton");
      instrall->set_label(gettext("Daily results for instrument"));
      instrall->set_margin(5);
      instrall->signal_clicked().connect([window, box, mw]
      {
	DownloadMenu dm(mw);
	dm.downlodSinglInstrAll(window, box);
      });
      grid->attach(*instrall, 0, 2, 1, 1);

      Gtk::Label *Dateserver = Gtk::make_managed<Gtk::Label>();
      Dateserver->set_name("dateServer");
      int dateserver = static_cast<int>(dateServer());
      Glib::ustring dateservstr = gettext("Last available date on server: ")
	  + af.togrigday(dateserver) + "." + af.togrigmonth(dateserver) + "."
	  + af.togrigyear(dateserver);
      Dateserver->set_text(dateservstr);
      Dateserver->set_margin(5);
      grid->attach(*Dateserver, 0, 3, 1, 1);

      Gtk::Label *Datehome = Gtk::make_managed<Gtk::Label>();
      Datehome->set_name("dateServer");
      int datehome = static_cast<int>(dateHome());
      Glib::ustring datehomestr = gettext("Last downloaded data, date: ")
	  + af.togrigday(datehome) + "." + af.togrigmonth(datehome) + "."
	  + af.togrigyear(datehome);
      Datehome->set_text(datehomestr);
      Datehome->set_margin(5);
      grid->attach(*Datehome, 0, 4, 1, 1);

      Gtk::Button *dealsdaily = Gtk::make_managed<Gtk::Button>();
      dealsdaily->set_name("confirmButton");
      dealsdaily->set_label(gettext("All transactions for current date"));
      dealsdaily->set_margin(5);
      dealsdaily->signal_clicked().connect([window, mw]
      {
	DownloadMenu dm(mw);
	dm.downloadDeals(window);
      });
      grid->attach(*dealsdaily, 0, 5, 1, 1);

      Gtk::Button *alld = Gtk::make_managed<Gtk::Button>();
      alld->set_name("confirmButton");
      alld->set_label(gettext("All results"));
      alld->set_margin(5);
      alld->signal_clicked().connect([window, mw]
      {
	DownloadMenu dm(mw);
	dm.downloadAll(window);
      });
      grid->attach(*alld, 0, 6, 1, 1);
      box->set_model(model);

      Gtk::TreeViewColumn *column = box->get_column(0);
      int x, y, h, w;
      column->cell_get_size(x, y, w, h);

      if(instruments.size() > 20)
	{
	  scroll->set_min_content_height(20 * h);
	}
      else
	{
	  scroll->set_min_content_height(instruments.size() * h);
	}
      Gtk::Requisition min, nat;
      box->get_preferred_size(min, nat);
      scroll->set_min_content_width(nat.get_width());

      window->signal_close_request().connect([window]
      {
	window->hide();
	delete window;
	return true;
      },
					     false);

      window->present();
    }

}

std::vector<std::tuple<std::string, std::string, std::string>>
DownloadMenu::formVectorInstr(int variant)
{
  std::vector<std::tuple<std::string, std::string, std::string>> result;

  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  if(variant == 1)
    {
      filename = filename + "/.Money/List";
    }
  else if(variant == 2)
    {
      filename = filename + "/.Money/ListF";
    }
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in);
  if(f.is_open())
    {
      std::tuple<int, int, int> indextup;
      std::get<0>(indextup) = -1;
      std::get<1>(indextup) = -1;
      std::get<2>(indextup) = -1;
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      std::tuple<std::string, std::string, std::string> restup;
	      if(count > 0)
		{
		  std::string tmp = line;
		  if(std::get<0>(indextup) >= 0)
		    {
		      int lc = 0;
		      while(lc <= std::get<0>(indextup) && !tmp.empty())
			{
			  std::string::size_type n;
			  n = tmp.find(";");
			  std::string ls;
			  if(n != std::string::npos)
			    {
			      ls = tmp.substr(0, n + std::string(";").size());
			      tmp.erase(0, ls.size());
			    }
			  else
			    {
			      ls = tmp;
			      tmp.clear();
			    }
			  ls = ls.substr(0, ls.find(";"));
			  if(lc == std::get<0>(indextup))
			    {
			      std::get<0>(restup) = ls;
			    }
			  lc++;
			}
		    }
		  tmp = line;
		  if(std::get<1>(indextup) >= 0)
		    {
		      int lc = 0;
		      while(lc <= std::get<1>(indextup) && !tmp.empty())
			{
			  std::string::size_type n;
			  n = tmp.find(";");
			  std::string ls;
			  if(n != std::string::npos)
			    {
			      ls = tmp.substr(0, n + std::string(";").size());
			      tmp.erase(0, ls.size());
			    }
			  else
			    {
			      ls = tmp;
			      tmp.clear();
			    }
			  ls = ls.substr(0, ls.find(";"));
			  if(lc == std::get<1>(indextup))
			    {
			      std::get<1>(restup) = ls;
			    }
			  lc++;
			}
		    }
		  tmp = line;
		  if(std::get<2>(indextup) >= 0)
		    {
		      int lc = 0;
		      while(lc <= std::get<2>(indextup) && !tmp.empty())
			{
			  std::string::size_type n;
			  n = tmp.find(";");
			  std::string ls;
			  if(n != std::string::npos)
			    {
			      ls = tmp.substr(0, n + std::string(";").size());
			      tmp.erase(0, ls.size());
			    }
			  else
			    {
			      ls = tmp;
			      tmp.clear();
			    }
			  ls = ls.substr(0, ls.find(";"));
			  if(lc == std::get<2>(indextup))
			    {
			      std::get<2>(restup) = ls;
			    }
			  lc++;
			}
		    }
		  result.push_back(restup);
		}
	      else
		{
		  std::string tmp = line;
		  int lc = 0;
		  while(!tmp.empty())
		    {
		      std::string::size_type n;
		      n = tmp.find(";");
		      std::string tl;
		      if(n != std::string::npos)
			{
			  tl = tmp.substr(0, n + std::string(";").size());
			  tmp.erase(0, tl.size());
			}
		      else
			{
			  tl = tmp;
			  tmp.clear();
			}
		      tl = tl.substr(0, tl.find(";"));
		      if(tl == "SECID")
			{
			  std::get<0>(indextup) = lc;
			}
		      else if(tl == "SHORTNAME")
			{
			  std::get<1>(indextup) = lc;
			}
		      else if(tl == "NAME")
			{
			  std::get<2>(indextup) = lc;
			}
		      if(std::get<0>(indextup) >= 0
			  && std::get<1>(indextup) >= 0
			  && std::get<2>(indextup) >= 0)
			{
			  break;
			}
		      lc++;
		    }
		}
	    }
	  count++;
	}
      f.close();
    }

  std::sort(result.begin(), result.end(), []
  (auto &el1, auto &el2)
    {
      return std::get<0>(el1) < std::get<0>(el2);
    });

  result.erase(std::unique(result.begin(), result.end(), []
  (auto &el1, auto &el2)
    {
      return std::get<0>(el1) == std::get<0>(el2);
    }),
	       result.end());

  std::sort(result.begin(), result.end(), []
  (auto el1, auto &el2)
    {
      std::string f = std::get<2>(el1);
      std::string s = std::get<2>(el2);
      AuxFunc af;
      af.stringToLower(f);
      af.stringToLower(s);
      return f < s;
    });

  return result;
}

std::vector<std::tuple<std::string, std::string>>
DownloadMenu::formVectorBoards()
{
  std::vector<std::tuple<std::string, std::string>> result;
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.Money/BoardsList";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      std::tuple<int, int> indextup;
      std::get<0>(indextup) = -1;
      std::get<1>(indextup) = -1;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(!line.empty())
	    {
	      if(count > 0)
		{
		  std::tuple<std::string, std::string> restup;
		  std::string tmp = line;
		  if(std::get<0>(indextup) >= 0)
		    {
		      int lc = 0;
		      while(lc <= std::get<0>(indextup) && !tmp.empty())
			{
			  std::string::size_type n;
			  n = tmp.find(";");
			  std::string ls;
			  if(n != std::string::npos)
			    {
			      ls = tmp.substr(0, n + std::string(";").size());
			      tmp.erase(0, ls.size());
			    }
			  else
			    {
			      ls = tmp;
			      tmp.clear();
			    }
			  ls = ls.substr(0, ls.find(";"));
			  if(lc == std::get<0>(indextup))
			    {
			      std::get<0>(restup) = ls;
			    }
			  lc++;
			}
		    }
		  tmp = line;
		  if(std::get<1>(indextup) >= 0)
		    {
		      int lc = 0;
		      while(lc <= std::get<1>(indextup) && !tmp.empty())
			{
			  std::string::size_type n;
			  n = tmp.find(";");
			  std::string ls;
			  if(n != std::string::npos)
			    {
			      ls = tmp.substr(0, n + std::string(";").size());
			      tmp.erase(0, ls.size());
			    }
			  else
			    {
			      ls = tmp;
			      tmp.clear();
			    }
			  ls = ls.substr(0, ls.find(";"));
			  if(lc == std::get<1>(indextup))
			    {
			      std::get<1>(restup) = ls;
			    }
			  lc++;
			}
		    }
		  result.push_back(restup);
		}
	      else
		{
		  std::string tmp = line;
		  int lcc = 0;
		  while(!tmp.empty())
		    {
		      std::string::size_type n;
		      n = tmp.find(";");
		      std::string lc;
		      if(n != std::string::npos)
			{
			  lc = tmp.substr(0, n + std::string(";").size());
			  tmp.erase(0, lc.size());
			}
		      else
			{
			  lc = tmp;
			  tmp.clear();
			}
		      lc = lc.substr(0, lc.find(";"));
		      if(lc == "boardid")
			{
			  std::get<0>(indextup) = lcc;
			}
		      else if(lc == "title")
			{
			  std::get<1>(indextup) = lcc;
			}
		      lcc++;
		    }
		}
	    }
	  count++;
	}

      f.close();
    }

  return result;
}

double
DownloadMenu::dateServer()
{
  double result = 0.0;
  std::string filename;
  AuxFunc af;
  filename = af.tempPath();
  filename = filename + "/Money/Dates";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in);
  if(f.is_open())
    {
      int count = 0;
      while(!f.eof())
	{
	  std::string line;
	  getline(f, line);
	  if(count == 3)
	    {
	      line.erase(0, line.find(";") + std::string(";").size());
	      int day, month, year;
	      std::string tmp1 = line;
	      tmp1 = tmp1.substr(0, tmp1.find("-"));
	      year = std::stoi(tmp1.c_str());
	      tmp1 = line;
	      tmp1.erase(0, tmp1.find("-") + std::string("-").size());
	      tmp1 = tmp1.substr(0, tmp1.find("-"));
	      month = std::stoi(tmp1.c_str());
	      tmp1 = line;
	      tmp1.erase(0, tmp1.find("-") + std::string("-").size());
	      tmp1.erase(0, tmp1.find("-") + std::string("-").size());
	      day = std::stoi(tmp1.c_str());
	      result = af.grigtojulian(day, month, year, 0, 0, 0.0);
	      break;
	    }
	  count++;
	}
      f.close();
    }

  return result;
}

double
DownloadMenu::dateHome()
{
  double result = 0.0;
  std::string filename;
  AuxFunc af;
  af.homePath(&filename);
  filename = filename + "/.Money/DateBegin";
  std::filesystem::path filepath = std::filesystem::u8path(filename);
  std::fstream f;
  f.open(filepath, std::ios_base::in);
  if(f.is_open())
    {
      std::string line;
      getline(f, line);
      f.close();
      std::stringstream strm;
      std::locale loc("C");
      strm.imbue(loc);
      strm << line;
      strm >> result;
    }

  return result;
}

void
DownloadMenu::instrSelection(const Gtk::TreeModel::Path &path,
			     Gtk::TreeViewColumn *column,
			     Glib::RefPtr<Gtk::ListStore> model,
			     Gtk::MenuButton *mb)
{
  const auto iter = model->get_iter(path);
  if(iter)
    {
      Glib::ustring val;
      iter->get_value(0, val);
      mb->set_label(val);
      mb->popdown();
    }
}

void
DownloadMenu::downloadAll(Gtk::Window *win)
{
  win->close();
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Downloading..."));
  window->set_deletable(false);
  window->set_name("downloadWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Downloading in progress..."));
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar>();
  progbar->set_show_text(true);
  progbar->set_name("progBar");
  progbar->set_margin(5);
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("closeButton");
  grid->attach(*cancel, 0, 2, 1, 1);

  int *cncl = new int(0);
  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  std::vector<std::tuple<std::string, std::string, std::string>> resv;
  resv = formVectorInstr(1);
  std::vector<std::tuple<std::string, std::string, std::string>> resfv;
  resfv = formVectorInstr(2);
  std::vector<std::tuple<std::string, std::string>> boards;
  boards = formVectorBoards();
  DownloadAll *dall = new DownloadAll(cncl, &resv, &resfv, &boards, dateHome());

  double *progbr = new double(0.0);
  int *svdt = new int(0);

  Gtk::Window *mw = Mwin;
  Glib::Dispatcher *progr_disp = new Glib::Dispatcher;
  Glib::Dispatcher *glob_err_disp = new Glib::Dispatcher;
  Glib::Dispatcher *canceled_disp = new Glib::Dispatcher;
  Glib::Dispatcher *net_err_disp = new Glib::Dispatcher;
  Glib::Dispatcher *save_date_disp = new Glib::Dispatcher;
  Glib::Dispatcher *fin_disp = new Glib::Dispatcher;

  progr_disp->connect([progbr, progbar]
  {
    double pr = *progbr;
    progbar->set_fraction(pr);
  });
  dall->progress = [progbr, progr_disp]
  (double pr)
    {
      *progbr = pr;
      progr_disp->emit();
    };

  glob_err_disp->connect(
      [window, mw, progr_disp, glob_err_disp, canceled_disp, net_err_disp,
       save_date_disp, fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.errorMessage();
	delete fin_disp;
	delete save_date_disp;
	delete net_err_disp;
	delete canceled_disp;
	delete progr_disp;
	delete glob_err_disp;
      });

  dall->globError = [glob_err_disp]
  {
    glob_err_disp->emit();
  };

  canceled_disp->connect(
      [window, progr_disp, glob_err_disp, canceled_disp, net_err_disp,
       save_date_disp, fin_disp]
      {
	window->close();
	delete fin_disp;
	delete save_date_disp;
	delete net_err_disp;
	delete progr_disp;
	delete glob_err_disp;
	delete canceled_disp;
      });

  dall->canceled = [canceled_disp]
  {
    canceled_disp->emit();
  };

  net_err_disp->connect(
      [window, mw, progr_disp, glob_err_disp, canceled_disp, net_err_disp,
       save_date_disp, fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.errorMessage();
	delete fin_disp;
	delete save_date_disp;
	delete progr_disp;
	delete glob_err_disp;
	delete canceled_disp;
	delete net_err_disp;
      });

  dall->netError = [net_err_disp]
  {
    net_err_disp->emit();
  };

  save_date_disp->connect([svdt, mw]
  {
    int dt = *svdt;
    DownloadMenu dm(mw);
    dm.saveDate(dt);
  });

  dall->saveDate = [save_date_disp, svdt]
  (int date)
    {
      *svdt = date;
      save_date_disp->emit();
    };

  dall->finished = [fin_disp]
  {
    fin_disp->emit();
  };

  fin_disp->connect(
      [window, mw, progr_disp, glob_err_disp, canceled_disp, net_err_disp,
       save_date_disp, fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.calcIndex();
	delete save_date_disp;
	delete progr_disp;
	delete glob_err_disp;
	delete canceled_disp;
	delete net_err_disp;
	delete fin_disp;
      });

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([dall, cncl, progbr, svdt]
  {
    dall->downloadAll();
    delete cncl;
    delete dall;
    delete progbr;
    delete svdt;
  });
  thr->detach();
  delete thr;
}

void
DownloadMenu::errorMessage()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_default_size(1, 1);
  window->set_title(gettext("Network error"));
  window->set_name("errorMessage");
  window->set_transient_for(*Mwin);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  grid->set_valign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Network error!"));
  label->set_halign(Gtk::Align::CENTER);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_halign(Gtk::Align::CENTER);
  close->set_margin(5);
  close->set_label(gettext("Close"));
  close->set_name("closeButton");
  grid->attach(*close, 0, 1, 1, 1);

  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
DownloadMenu::saveDate(int dt)
{
  std::fstream f;
  std::string line;
  AuxFunc af;
  af.homePath(&line);
  line = line + "/.Money/DateBegin";
  std::filesystem::path pathdb = std::filesystem::u8path(line);
  std::stringstream strm;
  std::locale loc("C");
  strm.str("");
  strm.clear();
  strm.imbue(loc);
  strm << std::fixed;
  strm << std::setprecision(0);
  strm << dt;
  line = strm.str();
  f.open(pathdb, std::ios_base::out | std::ios_base::binary);
  f.write(line.c_str(), line.size());
  f.close();
}

void
DownloadMenu::calcIndex()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("PPm calculation"));
  window->set_name("downloadWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);
  auto grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar>();
  progbar->set_show_text(true);
  progbar->set_name("progBar");
  progbar->set_margin(5);
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("closeButton");
  grid->attach(*cancel, 0, 2, 1, 1);

  int *cncl = new int(0);
  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  CalcIndex *calc = new CalcIndex(cncl);
  double *calci_progr = new double(0.0);
  Glib::ustring *lname = new Glib::ustring;

  Gtk::Window *mw = Mwin;
  Glib::Dispatcher *progr_disp = new Glib::Dispatcher;
  Glib::Dispatcher *opn_disp = new Glib::Dispatcher;
  Glib::Dispatcher *canceled_disp = new Glib::Dispatcher;
  Glib::Dispatcher *all_comp_disp = new Glib::Dispatcher;

  calc->progr = [calci_progr, progr_disp]
  (double element)
    {
      *calci_progr = element;
      progr_disp->emit();
    };

  progr_disp->connect([progbar, calci_progr]
  {
    progbar->set_fraction(*calci_progr);
  });

  calc->operationName = [lname, opn_disp]
  (Glib::ustring e)
    {
      *lname = e;
      opn_disp->emit();
    };
  opn_disp->connect([label, lname]
  {
    Glib::ustring nm = *lname;
    label->set_text(nm);
  });

  calc->canceled = [canceled_disp]
  {
    canceled_disp->emit();
  };

  canceled_disp->connect(
      [window, progr_disp, opn_disp, canceled_disp, all_comp_disp]
      {
	window->close();
	delete all_comp_disp;
	delete progr_disp;
	delete opn_disp;
	delete canceled_disp;
      });

  all_comp_disp->connect(
      [window, mw, progr_disp, opn_disp, canceled_disp, all_comp_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.finishMessage();
	delete progr_disp;
	delete opn_disp;
	delete canceled_disp;
	delete all_comp_disp;
      });

  calc->allComplet = [all_comp_disp]
  {
    all_comp_disp->emit();
  };

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([calc, cncl, calci_progr, lname]
  {
    calc->mainCalc();
    delete cncl;
    delete calc;
    delete calci_progr;
    delete lname;
  });
  thr->detach();
  delete thr;
}

void
DownloadMenu::downloadDeals(Gtk::Window *win)
{
  win->close();
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Transactions downloading"));
  window->set_deletable(false);
  window->set_name("downloadWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);
  auto grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Transaction downloading progress:"));
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *progbar = Gtk::make_managed<Gtk::ProgressBar>();
  progbar->set_show_text(true);
  progbar->set_margin(5);
  progbar->set_name("progBar");
  grid->attach(*progbar, 0, 1, 1, 1);

  Gtk::Button *cancel = Gtk::make_managed<Gtk::Button>();
  cancel->set_label(gettext("Cancel"));
  cancel->set_margin(5);
  cancel->set_halign(Gtk::Align::CENTER);
  cancel->set_name("closeButton");
  grid->attach(*cancel, 0, 2, 1, 1);

  double *frc = new double(0.0);
  int *cncl = new int(0);
  std::vector<std::tuple<std::string, std::string, std::string>> resv;
  resv = formVectorInstr(1);
  std::vector<std::tuple<std::string, std::string, std::string>> resfv;
  resfv = formVectorInstr(2);
  std::vector<std::tuple<std::string, std::string>> boards;
  boards = formVectorBoards();
  DownloadDeals *ddls = new DownloadDeals(cncl, &resv, &resfv, &boards, 0);
  cancel->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  Gtk::Window *mw = Mwin;
  Glib::Dispatcher *nodelas_disp = new Glib::Dispatcher;
  Glib::Dispatcher *neterror_disp = new Glib::Dispatcher;
  Glib::Dispatcher *canceled_disp = new Glib::Dispatcher;
  Glib::Dispatcher *progr_disp = new Glib::Dispatcher;
  Glib::Dispatcher *fin_disp = new Glib::Dispatcher;

  ddls->noDeals = [nodelas_disp]
  {
    nodelas_disp->emit();
  };

  nodelas_disp->connect(
      [window, mw, nodelas_disp, neterror_disp, canceled_disp, progr_disp,
       fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.noDeals(1);
	delete fin_disp;
	delete progr_disp;
	delete canceled_disp;
	delete neterror_disp;
	delete nodelas_disp;
      });

  ddls->netError = [neterror_disp]
  {
    neterror_disp->emit();
  };

  neterror_disp->connect(
      [window, mw, nodelas_disp, neterror_disp, canceled_disp, progr_disp,
       fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.noDeals(2);
	delete fin_disp;
	delete progr_disp;
	delete canceled_disp;
	delete nodelas_disp;
	delete neterror_disp;
      });

  ddls->canceled = [canceled_disp]
  {
    canceled_disp->emit();
  };

  canceled_disp->connect(
      [window, nodelas_disp, neterror_disp, canceled_disp, progr_disp, fin_disp]
      {
	window->close();
	delete fin_disp;
	delete progr_disp;
	delete nodelas_disp;
	delete neterror_disp;
	delete canceled_disp;
      });

  ddls->progr = [frc, progr_disp]
  (double el)
    {
      *frc = el;
      progr_disp->emit();
    };

  progr_disp->connect([frc, progbar]
  {
    progbar->set_fraction(*frc);
  });

  ddls->finished = [fin_disp]
  {
    fin_disp->emit();
  };

  fin_disp->connect(
      [window, mw, nodelas_disp, neterror_disp, canceled_disp, progr_disp,
       fin_disp]
      {
	window->close();
	DownloadMenu dm(mw);
	dm.finishMessage();
	delete progr_disp;
	delete nodelas_disp;
	delete neterror_disp;
	delete canceled_disp;
	delete fin_disp;
      });

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([ddls, frc, cncl]
  {
    ddls->downloadAll();
    delete frc;
    delete cncl;
    delete ddls;
  });
  thr->detach();
  delete thr;
}

void
DownloadMenu::noDeals(int sel)
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_default_size(1, 1);
  window->set_transient_for(*Mwin);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  if(sel == 1)
    {
      label->set_text(gettext("No transactions today"));
      window->set_name("downloadWindow");
      window->set_title(gettext("Message"));
      window->set_default_size(1, 1);
    }
  else
    {
      label->set_text(gettext("Network error"));
      window->set_title(gettext("Error!"));
      window->set_name("errorMessage");
      window->set_default_size(1, 1);
    }
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::Button *button = Gtk::make_managed<Gtk::Button>();
  button->set_label(gettext("Close"));
  button->set_margin(5);
  button->set_halign(Gtk::Align::CENTER);
  button->set_name("closeButton");

  grid->attach(*button, 0, 1, 1, 1);

  button->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
DownloadMenu::downlodSinglInstrAll(Gtk::Window *win, Gtk::TreeView *tv)
{
  std::vector<std::tuple<std::string, std::string, std::string>> instruments;
  Glib::RefPtr<Gtk::TreeSelection> sel = tv->get_selection();
  if(sel)
    {
      auto iter = sel->get_selected();
      Glib::ustring code, fnm;
      if(iter)
	{
	  iter->get_value(1, fnm);
	  iter->get_value(2, code);
	  instruments.push_back(
	      std::make_tuple(std::string(code), std::string(fnm), ""));
	}
      else
	{
	  auto row = tv->get_model()->children()[0];
	  if(row)
	    {
	      row.get_value(1, fnm);
	      row.get_value(2, code);
	      instruments.push_back(
		  std::make_tuple(std::string(code), std::string(fnm), ""));
	    }
	}
    }
  win->close();

  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Downloading..."));
  window->set_name("downloadWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Downloading in progress..."));
  label->set_halign(Gtk::Align::CENTER);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *prgb = Gtk::make_managed<Gtk::ProgressBar>();
  prgb->set_margin(5);
  prgb->set_show_text(false);
  prgb->set_name("progBar");
  grid->attach(*prgb, 0, 1, 1, 1);

  Gtk::Button *stop = Gtk::make_managed<Gtk::Button>();
  stop->set_margin(5);
  stop->set_label(gettext("Cancel"));
  stop->set_halign(Gtk::Align::CENTER);
  stop->set_name("closeButton");
  grid->attach(*stop, 0, 2, 1, 1);

  int *cncl = new int(0);
  stop->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  std::vector<std::tuple<std::string, std::string, std::string>> vectf;
  vectf = formVectorInstr(2);
  AuxFunc af;
  double bd = af.grigtojulian(23, 3, 1997, 0, 0, 0.0);
  std::vector<std::tuple<std::string, std::string>> boards;
  boards = formVectorBoards();
  DownloadAll *all = new DownloadAll(cncl, &instruments, &vectf, &boards, bd);

  Glib::Dispatcher *pulse_disp = new Glib::Dispatcher;
  Glib::Dispatcher *fin_disp = new Glib::Dispatcher;
  Glib::Dispatcher *glob_err_disp = new Glib::Dispatcher;
  Glib::Dispatcher *canceled_disp = new Glib::Dispatcher;
  Gtk::Window *mw = Mwin;
  all->pulse = [pulse_disp]
  {
    pulse_disp->emit();
  };

  pulse_disp->connect([prgb]
  {
    prgb->pulse();
  });

  all->globError = [glob_err_disp]
  {
    glob_err_disp->emit();
  };

  glob_err_disp->connect(
      [window, mw, fin_disp, pulse_disp, glob_err_disp, canceled_disp]
      {
	DownloadMenu dm(mw);
	dm.errorMessage();
	window->close();
	delete pulse_disp;
	delete fin_disp;
	delete canceled_disp;
	delete glob_err_disp;
      });

  all->canceled = [canceled_disp]
  {
    canceled_disp->emit();
  };

  canceled_disp->connect(
      [window, fin_disp, pulse_disp, glob_err_disp, canceled_disp]
      {
	window->close();
	delete pulse_disp;
	delete glob_err_disp;
	delete fin_disp;
	delete canceled_disp;
      });

  all->netError = [glob_err_disp]
  {
    glob_err_disp->emit();
  };

  all->finished = [fin_disp]
  {
    fin_disp->emit();
  };

  fin_disp->connect(
      [window, mw, fin_disp, pulse_disp, glob_err_disp, canceled_disp]
      {
	DownloadMenu dm(mw);
	dm.finishMessage();
	window->close();
	delete pulse_disp;
	delete glob_err_disp;
	delete canceled_disp;
	delete fin_disp;
      });

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();

  std::thread *thr = new std::thread([all, cncl]
  {
    all->downloadAll();
    delete all;
    delete cncl;
  });
  thr->detach();
  delete thr;
}

void
DownloadMenu::finishMessage()
{
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Downloading completed"));
  window->set_name("finishMessage");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::CENTER);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Downloading completed!"));
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);
  label->set_size_request(-1, -1);

  Gtk::Button *close = Gtk::make_managed<Gtk::Button>();
  close->set_label(gettext("Close"));
  close->set_margin(5);
  close->set_halign(Gtk::Align::CENTER);
  close->set_name("closeButton");
  close->signal_clicked().connect(sigc::mem_fun(*window, &Gtk::Window::close));
  grid->attach(*close, 0, 1, 1, 1);

  window->signal_close_request().connect([window, this]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
}

void
DownloadMenu::downlodSinglInstrDeals(Gtk::Window *win, Gtk::TreeView *tv)
{
  std::vector<std::tuple<std::string, std::string, std::string>> instruments;
  Glib::RefPtr<Gtk::TreeSelection> sel = tv->get_selection();
  if(sel)
    {
      auto iter = sel->get_selected();
      Glib::ustring code, fnm;
      if(iter)
	{
	  iter->get_value(1, fnm);
	  iter->get_value(2, code);
	  instruments.push_back(
	      std::make_tuple(std::string(code), std::string(fnm), ""));
	}
      else
	{
	  auto row = tv->get_model()->children()[0];
	  if(row)
	    {
	      row.get_value(1, fnm);
	      row.get_value(2, code);
	      instruments.push_back(
		  std::make_tuple(std::string(code), std::string(fnm), ""));
	    }
	}
    }
  win->close();
  Gtk::Window *window = new Gtk::Window;
  window->set_application(Mwin->get_application());
  window->set_title(gettext("Downloading..."));
  window->set_name("downloadWindow");
  window->set_transient_for(*Mwin);
  window->set_default_size(1, 1);

  Gtk::Grid *grid = Gtk::make_managed<Gtk::Grid>();
  grid->set_halign(Gtk::Align::FILL);
  grid->set_valign(Gtk::Align::FILL);
  window->set_child(*grid);

  Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
  label->set_text(gettext("Downloading in progress..."));
  label->set_halign(Gtk::Align::CENTER);
  label->set_margin(5);
  grid->attach(*label, 0, 0, 1, 1);

  Gtk::ProgressBar *prgb = Gtk::make_managed<Gtk::ProgressBar>();
  prgb->set_margin(5);
  prgb->set_name("progBar");
  prgb->set_show_text(false);
  grid->attach(*prgb, 0, 1, 1, 1);

  Gtk::Button *stop = Gtk::make_managed<Gtk::Button>();
  stop->set_margin(5);
  stop->set_label(gettext("Cancel"));
  stop->set_halign(Gtk::Align::CENTER);
  stop->set_name("closeButton");
  grid->attach(*stop, 0, 2, 1, 1);

  int *cncl = new int(0);
  stop->signal_clicked().connect([cncl]
  {
    *cncl = 1;
  });

  std::vector<std::tuple<std::string, std::string, std::string>> vectf;
  vectf = formVectorInstr(2);
  std::vector<std::tuple<std::string, std::string>> boards;
  boards = formVectorBoards();
  DownloadDeals *all = new DownloadDeals(cncl, &instruments, &vectf, &boards,
					 1);

  Glib::Dispatcher *pulse_disp = new Glib::Dispatcher;
  Glib::Dispatcher *nodeals_disp = new Glib::Dispatcher;
  Glib::Dispatcher *net_err_disp = new Glib::Dispatcher;
  Glib::Dispatcher *canceled_disp = new Glib::Dispatcher;
  Glib::Dispatcher *fin_disp = new Glib::Dispatcher;
  Gtk::Window *mw = Mwin;

  all->pulse = [pulse_disp]
  {
    pulse_disp->emit();
  };

  pulse_disp->connect([prgb]
  {
    prgb->pulse();
  });

  all->netError = [net_err_disp]
  {
    net_err_disp->emit();
  };

  net_err_disp->connect(
      [window, mw, pulse_disp, canceled_disp, net_err_disp, fin_disp,
       nodeals_disp]
      {
	DownloadMenu dm(mw);
	dm.errorMessage();
	window->close();
	delete pulse_disp;
	delete nodeals_disp;
	delete fin_disp;
	delete canceled_disp;
	delete net_err_disp;
      });

  all->canceled = [canceled_disp]
  {
    canceled_disp->emit();
  };

  canceled_disp->connect(
      [window, pulse_disp, canceled_disp, net_err_disp, fin_disp, nodeals_disp]
      {
	window->close();
	delete pulse_disp;
	delete net_err_disp;
	delete nodeals_disp;
	delete fin_disp;
	delete canceled_disp;
      });

  all->finished = [fin_disp]
  {
    fin_disp->emit();
  };

  fin_disp->connect(
      [window, mw, pulse_disp, canceled_disp, net_err_disp, fin_disp,
       nodeals_disp]
      {
	DownloadMenu dm(mw);
	dm.finishMessage();
	window->close();
	delete pulse_disp;
	delete canceled_disp;
	delete net_err_disp;
	delete nodeals_disp;
	delete fin_disp;
      });

  all->noDeals = [nodeals_disp]
  {
    nodeals_disp->emit();
  };

  nodeals_disp->connect(
      [window, mw, pulse_disp, canceled_disp, net_err_disp, fin_disp,
       nodeals_disp]
      {
	DownloadMenu dm(mw);
	dm.noDeals(1);
	window->close();
	delete pulse_disp;
	delete canceled_disp;
	delete net_err_disp;
	delete fin_disp;
	delete nodeals_disp;
      });

  window->signal_close_request().connect([window]
  {
    window->hide();
    delete window;
    return true;
  },
					 false);

  window->present();
  std::thread *thr = new std::thread([all, cncl]
  {
    all->downloadAll();
    delete all;
    delete cncl;
  });
  thr->detach();
  delete thr;
}
