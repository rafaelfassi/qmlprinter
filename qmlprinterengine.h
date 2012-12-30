/**************************************************************************
**
** This file is part of QmlPrinter
**
** Copyright (c) 2012 Rafael Fassi Lobão
**
** Contact: www.fassi.com.br - rafael@fassi.com.br
**
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/


#ifndef QMLPRINTERENGINE_H
#define QMLPRINTERENGINE_H

#include <QGraphicsEffect>
#include <QPointer>
#include <QPicture>
#include <QList>
#include <QHash>

/**
  @brief Macro para criar automaticamente os getters e setters das variaveis.
  @param type Tipo de variável.
  @param name Nome da variável.
*/
#define DECLARE_VAR(type, name) \
private: type m_##name; \
public: \
void set_##name(const type& var) \
{\
    m_##name = var;\
}\
\
type name()const \
{\
    return m_##name; \
}\

class QDeclarativeItem;
class QPointF;

class GraphicsEffect : public QGraphicsEffect
{
public:
    void drawGraphicsEffect(QDeclarativeItem *item, QPainter *painter);

protected:
    virtual void draw(QPainter *painter);
};

struct QmlItem
{
    QString objectName;
    QString className;
    QPicture picture;
    QRectF rect;
    QRectF printRect;
    bool visible;
    int page;
    int index;
    bool breakAlways;
    bool breakByOffset;
    qreal breakOffset;
    int header;
    bool resize;
};

/**
    @class  QmlPrinterEngine
    @brief Classe para impressão de objetos qml carregados na memória.

    @section sec_decricao QmlPrinterEngine
    Esta clase permite criar relatórios através de objetos QML carregados na memória.
    Inicialmente deve-se passar o objeto desejado para a classe. Após isso o objeto será parseado
    dinamicamente, obtendo suas imagens, posições na tela e outras propriedades. Os objetos são parseados
    seguindo sua ordem de criação no QML. Alterar a propriedade "z" do objeto implica diretamente em sua
    ordem de criação.

    @subsection ssec_diretivasimpressao Diretivas de impressão
    Diretivas de impressão são passadas pela propriedade objectName dos objetos QML. Elas servem para
    configurar o comportamento de impressão dos objetos. A seguir são listadas as diretivas de impressão
    atualmente suportadas pela engine.

    "_NoPrint_" \n
    Não imprime o objeto e nem seus filhos.

    "_NoPaint_" \n
    Não desenha o objeto, porém desenha seus filhos.

    "_Group_" \n
    A principal utilidade de um grupo é não permitir que ocorra uma quebra de página em algum elemento
    interno a ele. Objetos agrupados tendem a consumir menos memória do que objetos soltos, pois são
    transformados em um único objeto ao serem parseados.

    "_Header:id_" \n
    Define o objeto como um cabeçalho. Podem existir diversos cabeçalhos, então para cada cabeçalho
    deve-se definir um id que pode variar de 1...n.
    Um cabeçalho ja é um grupo por padrão, então uma diretiva igual a esta "_Header:1_Group_" é redundante.

    "_StartHeader:id_" \n
    Inicia o cabeçalho correspondente ao id.
    O cabeçalho será colocado a cada quebra de página até que outro cabeçalho seja definido
    ou "_EndHeader_" seja encontrado. Este comando é lido por ordem posicional dos objetos.

    "_EndHeader_" \n
    Encerra qualquer cabeçalho iniciado. Este comando é lido por ordem posicional dos objetos.

    "_PageBreak_" \n
    Produz uma quebra de página incondicional.
    Funciona somente em itens visíveis e é lido por ordem posicional dos objetos.

    "_PageBreak:height_" \n
    Somente quebra a pagina se após o objeto atual, height superar o tamanho da pagina.
    Funciona somente em itens visíveis e é lido por ordem posicional dos objetos.

    "_PrintScreen_" \n
    Em alguns casos, elementos visuais qml não reimplementam a função virtual paint. Em vez disso,
    eles herdam QDeclarativePaintedItem e reimplementam a função drawContents() que armazena o
    desenho do elemento em cache e o pinta somente quando a tela solicitar um refresh. Um exemplo
    de elemento com esse comportamento é o TextEdit. Em casos onde isso acontece ou casos onde a
    combinação das camadas gráficas não reflete a imagem original no relatório, esta diretiva pode
    ser usada para que a imagem seja realmente um print do elemento na tela. Como desvantagem, a
    imagem será um pixmap. Os objetos filhos não serão parseados, pois a imagem é obtida do elemento
    como um todo, incluindo seus itens internos. Devido a esse fato, o tamanho e posicionamento da
    imagem é baseado somente no elemento pai, implicando que nenhum objeto filho pode estar posicionado
    em uma área fora do mesmo.

    @author Rafael Fassi Lobao
    @date 14/07/2012
*/
class QmlPrinterEngine : public QObject
{

    Q_OBJECT

public:

    QmlPrinterEngine(QObject *obj);
    virtual ~QmlPrinterEngine() {}

    /**
        @brief Reseta as configurações setadas na engine.
    */
    Q_INVOKABLE void reset();

    /**
        @brief Seta o objeto qml a ser impresso.
        @param item Objeto qml a ser impresso.
    */
    Q_INVOKABLE void setItemSource(QDeclarativeItem *item);

    /**
        @brief Quando autoScale for false, esta função seta a escala de impressão.
        @param scale Escala de impressao.
        @see setAutoScale()
    */
    Q_INVOKABLE void setScale(qreal scale);

    /**
        @brief Seta se a escala de impressão será automática.
        @param autoScale Caso seja true a escala será ajustada ao papel da impressora.
        @see setScale()
    */
    Q_INVOKABLE void setAutoScale(bool autoScale) { m_autoScale = autoScale; }

    /**
        @brief Seta o nome do relatório.
        @param reportName Nome do relatorio.
    */
    Q_INVOKABLE void setReportName(QString reportName) { m_reportName = reportName; }

    /**
        @brief Retorna o nome do relatório.
        @return Nome do relatorio.
    */
    Q_INVOKABLE QString reportName() { return m_reportName; }

    /**
        @brief Imprime o relatório.
        @param pdfFormat Indica se a saída será impressa em um arquivo pdf.
        @param item Objeto qml a ser impresso.
        @see setItemSource()
    */
    Q_INVOKABLE void print(bool pdfFormat = false, QDeclarativeItem *item = 0);

    /**
        @brief Seta a opção para que seja enviado o número total de páginas a função que cria o rodapé.
        @param countPagesInFooter Indica se o numer total de páginas sera enviado para a função que cria o rodapé.
        @see pageFooter()
    */
    void setCountPagesInFooter(bool countPagesInFooter) { m_countPagesInFooter = countPagesInFooter; }


private slots:

    /**
        @brief Slot de impressão, chamado quando é solicitado o desenho do relatório nas páginas.
        @param printer Componente de impressão no qual o relatório será desenhado.
    */
    void printDocument(QPrinter *printer);

protected:

    /**
        @brief Função virtual que permite um pre-processamento do item antes do mesmo ser parsiado.
        @param item Item a ser parsiado.
        @see posProcessItem()
        @see parseDone()
    */
    virtual void preProcessItem(QDeclarativeItem *item) { Q_UNUSED(item) }

    /**
        @brief Função virtual que permite um pos-processamento do item apos o mesmo ser parsiado.
        @param item Item parsiado.
        @see preProcessItem()
        @see parseDone()
    */
    virtual void posProcessItem(QDeclarativeItem *item) { Q_UNUSED(item) }

    /**
        @brief Função virtual chamada apos todos os itens terem sido parsiados.
    */
    virtual void parseDone() {}

    /**
        @brief Função virtual que permite adicionar um cabeçalho inicial ao relatório.
        @param pageWidth largura da página.
        @see firstHeader()
        @see lastInfo()
        @return Imagem do cabecalho.
    */
    virtual QPicture firstHeader(qreal pageWidth) { Q_UNUSED(pageWidth) return QPicture(); }

    /**
        @brief Função virtual que permite adicionar rodapé as páginas do relatório.
        @param pageWidth Largura da página.
        @param page Página atual.
        @param count Numero total de páginas.
        @see firstHeader()
        @see lastInfo()
        @see setCountPagesInFooter()
        @return Imagem do rodapé.
        @note
        O número total de paginas só é informado caso countPagesInFooter esteja ativo. Ativar esta opção
        consome mais processamento, pois esta funcao sera chamada duas vezes, passando zero na primeira
        e o numero total de páginas na segunda.
    */
    virtual QPicture pageFooter(qreal pageWidth, int page, int count = 0)
    { Q_UNUSED(pageWidth) Q_UNUSED(page) Q_UNUSED(count) return QPicture(); }

    /**
        @brief Função virtual que permite adicionar uma última informação ao relatório.
        @param pageWidth Largura da página.
        @return Imagem da última informação do relatório.
    */
    virtual QPicture lastInfo(qreal pageWidth){ Q_UNUSED(pageWidth) return QPicture(); }

private:

    /**
        @brief Limpa os dados gerados para a driação do relatório.
    */
    void clearData();

    /**
        @brief Limpa itens temporarios apendados a lista, como rodapés e cabeçalhos.
    */
    void clearTemporaryItens();

    /**
        @brief Recebe um objeto qml e faz um parse recursivo de seus filhos, os tranformando
               em itens e apendando a lista.
        @param item Objeto qml a ser parseado.
        @param group Indica se o item pertence a um grupo.
        @param opacity Opacidade a ser aplicada ao item.
        @param rotation Rotação a ser aplicada ao item.
        @param scale Escala a ser aplicada ao item.
    */
    void parseItem(QDeclarativeItem *item, bool group = false,
                   qreal opacity = 1.0, qreal rotation = 0, qreal scale = 1.0);

    /**
        @brief Cria um grupo, transormando um objeto qml e todos os seus filhos em um único item.
        @param item Objeto qml que será usado para criar o grupo.
        @param rect Após o grupo ser criado, rect conterá o ramanho e cordenadas do grupo.
        @param opacity Opacidade a ser aplicada ao grupo.
        @param rotation Rotação a ser aplicada ao grupo.
        @param scale Escala a ser aplicada ao grupo.
        @return Retorna a imagem do grupo.
    */
    QPicture createGroup(QDeclarativeItem *item, QRectF &rect,
                         qreal opacity = 1.0, qreal rotation = 0, qreal scale = 1.0);

    /**
        @brief Realiza um pre-processamento na pagina onde são feitos ajustes iniciais como
               definicao da escala, ajuste das margins entre outros.
        @param printer Componente de impressão no qual o relatório será desenhado.
    */
    void preProcessPages(QPrinter *printer);

    /**
        @brief Realiza o processamento das páginas. Sua principal função é definir em que pagina
               cada item será impresso e adicionar cabeçalho e rodapé a lista.
        @param printer Componente de impressão no qual o relatório será desenhado.
        @return retorna o número total de páginas do relatório.
    */
    int processPages(QPrinter *printer);

    /**
        @brief Função usada pela função @ref processPages() para determinar se a página
               deve ser quebrada.
        @param qmlitem Item atual sendo processado.
        @param printer Componente de impressão no qual o relatório será desenhado.
        @return retorna true se a página deve ser quebrada, caso contrário retorna false.
    */
    bool breakPage(QmlItem &qmlitem, QPrinter *printer);

    /**
        @brief Função para desenhar as margens do documento.
               Deve ser usada somente para testes de conteúdo.
        @param printer Componente de impressão no qual o relatório será desenhado.
        @param painter Componente que desenho no QPrinter.
    */
    void drawMargins(QPrinter *printer, QPainter *painter);

    /**
        @brief Cria o item de rodapé da página.
        @param printer Componente de impressão no qual o relatório será desenhado.
        @param page Número da página que irá usar o rodapé.
        @return Item contendo imagem coordenadas e propriedades do rodapé.
    */
    QmlItem createPageFooter(QPrinter *printer, int page);

    /**
        @brief Cria o item de última informação do relatório.
        @param printer Componente de impressão no qual o relatório será desenhado.
        @param top Posição de topo do item.
        @return Item contendo imagem coordenadas e propriedades da última informação.
    */
    QmlItem createLastInfo(QPrinter *printer, qreal top);

    /**
        @brief Função usada pelo qSort para fazer ordenação pelo topo do item.
        @param qmlItem1 Primeiro item.
        @param qmlItem1 Segundo item.
        @return Retorna true se o primeiro item é menor que o segundo, caso contrário, retorna false.
    */
    static bool lessThanToTop(QmlItem &qmlItem1, QmlItem &qmlItem2);

    /**
        @brief Função usada pelo qSort para fazer ordenação pelo index do item.
        @param qmlItem1 Primeiro item.
        @param qmlItem1 Segundo item.
        @return Retorna true se o primeiro item é menor que o segundo, caso contrário, retorna false.
    */
    static bool lessThanToIndex(QmlItem &qmlItem1, QmlItem &qmlItem2);

    QPointer<QDeclarativeItem> m_item;
    QList<QmlItem> m_qmlItens;
    QList<QmlItem> m_group;
    QHash<int, QmlItem> m_headers;
    QPicture m_firstHeader;
    int m_index;
    qreal m_exMargin;
    bool m_drawMargins;
    qreal m_scale;
    qreal m_aScale;
    bool m_autoScale;
    QRectF m_contentRect;
    qreal m_footerHeight;
    int m_headerAt;
    QString m_reportName;
    bool m_countPagesInFooter;

    // TODO - Fazer os getters e setters manualmente para incluir a documentacao
    DECLARE_VAR(qreal, topMargin)
    DECLARE_VAR(qreal, leftMargin)
    DECLARE_VAR(bool, autoTopMargin)
    DECLARE_VAR(bool, autoLeftMargin)
    DECLARE_VAR(bool, aligVertical)
    DECLARE_VAR(bool, aligHorizontal)
    DECLARE_VAR(bool, resizeFirstHeader)
    DECLARE_VAR(bool, resizeFooter)
    DECLARE_VAR(bool, resizeLastInfo)
    DECLARE_VAR(bool, lastInfoInBottom)

};

#endif // QMLPRINTERENGINE_H
